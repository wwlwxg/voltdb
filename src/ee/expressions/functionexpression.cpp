/* This file is part of VoltDB.
 * Copyright (C) 2008-2012 VoltDB Inc.
 *
 * VoltDB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VoltDB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VoltDB.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "common/common.h"
#include "common/serializeio.h"
#include "common/valuevector.h"

#include "expressions/abstractexpression.h"
#include "expressions/expressionutil.h"

#include <string>
#include <cassert>

namespace voltdb {

namespace functionexpressions {

/*
 * Constant (no parameter) function. (now, random)
 */
template <ExpressionType E>
class ConstantFunctionExpression : public AbstractExpression {
public:
    ConstantFunctionExpression(const std::string& sqlName, const std::string& uniqueName)
        : AbstractExpression(E) {
    };

    NValue eval(const TableTuple *, const TableTuple *) const {
        return NValue::callConstant<E>();
    }

    std::string debugInfo(const std::string &spacer) const {
        return (spacer + "ConstantFunctionExpression " + expressionToString(getExpressionType()));
    }
};

/*
 * Unary functions. (abs, upper, lower)
 */

template <ExpressionType E>
class UnaryFunctionExpression : public AbstractExpression {
    AbstractExpression * const m_child;
public:
    UnaryFunctionExpression(AbstractExpression *child)
        : AbstractExpression(E)
        , m_child(child) {
    }

    virtual ~UnaryFunctionExpression() {
        delete m_child;
    }

    NValue eval(const TableTuple *tuple1, const TableTuple *tuple2) const {
        assert (m_child);
        return (m_child->eval(tuple1, tuple2)).callUnary<E>();
    }

    std::string debugInfo(const std::string &spacer) const {
        return (spacer + "UnaryFunctionExpression " + expressionToString(getExpressionType()));
    }
};

/*
 * N-ary functions.
 */
template <ExpressionType E>
class GeneralFunctionExpression : public AbstractExpression {
public:
    GeneralFunctionExpression(const std::vector<AbstractExpression *>& args)
        : AbstractExpression(E), m_args(args) {}

    virtual ~GeneralFunctionExpression() {
        size_t i = m_args.size();
        while (i--) {
            delete m_args[i];
        }
        delete &m_args;
    }

    NValue eval(const TableTuple *tuple1, const TableTuple *tuple2) const {
        std::vector<NValue> nValue(m_args.size());
        for (int i = 0; i < m_args.size(); ++i) {
            nValue[i] = m_args[i]->eval(tuple1, tuple2);
        }
        return NValue::call<E>(nValue);
    }

    std::string debugInfo(const std::string &spacer) const {
        return (spacer + "GeneralFunctionExpression " + expressionToString(getExpressionType()));
    }

private:
    const std::vector<AbstractExpression *>& m_args;
};

}
}

using namespace voltdb;
using namespace functionexpressions;

namespace voltdb {

/** implement a forced SQL ERROR function (for test and example purposes) for either integer or string types **/
template<> inline NValue NValue::callUnary<EXPRESSION_TYPE_FUNCTION_SQL_ERROR>() const {
    int64_t intValue = -1;
    char buffer[1024];
    const char* msgcode;
    const char* msgtext;
    const ValueType type = getValueType();
    if (type == VALUE_TYPE_VARCHAR) {
        const int32_t valueUTF8Length = getObjectLength();
        const char *valueChars = reinterpret_cast<char*>(getObjectValue());
        snprintf(buffer, std::min((int32_t)sizeof(buffer), valueUTF8Length), "%s", valueChars);
        msgcode = SQLException::nonspecific_error_code_for_error_forced_by_user;
        msgtext = buffer;
    } else {
        intValue = castAsBigIntAndGetValue(); // let cast throw if invalid
        snprintf(buffer, sizeof(buffer), "%ld", (long) intValue);
        msgcode = buffer;
        msgtext = SQLException::specific_error_specified_by_user;
    }
    if (intValue != 0) {
        throw SQLException(msgcode, msgtext);
    }
    return *this;
}

/** implement the 2-argument forced SQL ERROR function (for test and example purposes) */
template<> inline NValue NValue::call<EXPRESSION_TYPE_FUNCTION_SQL_ERROR>(const std::vector<NValue>& arguments) {
    assert(arguments.size() == 2);
    int64_t intValue = -1;
    char buffer[1024];
    char buffer2[1024];
    const char* msgcode;
    const char* msgtext;

    const NValue& codeArg = arguments[0];
    if (codeArg.isNull()) {
        msgcode = SQLException::nonspecific_error_code_for_error_forced_by_user;
    } else {
        intValue = codeArg.castAsBigIntAndGetValue(); // let cast throw if invalid
        snprintf(buffer, sizeof(buffer), "%ld", (long) intValue);
        msgcode = buffer;
    }

    const NValue& strValue = arguments[1];
    if (strValue.isNull()) {
        msgtext = "";
    } else {
        if (strValue.getValueType() != VALUE_TYPE_VARCHAR) {
            throwCastSQLException (strValue.getValueType(), VALUE_TYPE_VARCHAR);
        }

        const int32_t valueUTF8Length = strValue.getObjectLength();
        char *valueChars = reinterpret_cast<char*>(strValue.getObjectValue());
        snprintf(buffer2, std::min((int32_t)sizeof(buffer), valueUTF8Length), "%s", valueChars);
        msgtext = buffer2;
    }
    if (intValue != 0) {
        throw SQLException(msgcode, msgtext);
    }
    return codeArg;
}


AbstractExpression*
ExpressionUtil::functionFactory(ExpressionType et, const std::vector<AbstractExpression*>* arguments) {
    assert(arguments);
    AbstractExpression* ret = 0;
    size_t nArgs = arguments->size();
    switch(nArgs) {
    case 0:
        // ret = new ConstantFunctionExpression<???>();
        delete arguments;
        break;
    case 1:
        if (et == EXPRESSION_TYPE_FUNCTION_ABS) {
            ret = new UnaryFunctionExpression<EXPRESSION_TYPE_FUNCTION_ABS>((*arguments)[0]);
        } else if (et == EXPRESSION_TYPE_FUNCTION_SQL_ERROR) {
            ret = new UnaryFunctionExpression<EXPRESSION_TYPE_FUNCTION_SQL_ERROR>((*arguments)[0]);
        }
        delete arguments;
        break;
    default:
        // GeneralFunctions delete the arguments container when through with it.
        if (et == EXPRESSION_TYPE_FUNCTION_SUBSTRING_FROM) {
            ret = new GeneralFunctionExpression<EXPRESSION_TYPE_FUNCTION_SUBSTRING_FROM>(*arguments);
        } else if (et == EXPRESSION_TYPE_FUNCTION_SUBSTRING_FROM_FOR) {
            ret = new GeneralFunctionExpression<EXPRESSION_TYPE_FUNCTION_SUBSTRING_FROM_FOR>(*arguments);
        } else if (et == EXPRESSION_TYPE_FUNCTION_SQL_ERROR) {
            ret = new GeneralFunctionExpression<EXPRESSION_TYPE_FUNCTION_SQL_ERROR>(*arguments);
        }
    }
    return ret;
}

}

