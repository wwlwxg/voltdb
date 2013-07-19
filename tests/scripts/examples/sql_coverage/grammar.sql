{_numfun |= ""}
{_numfun |= "ABS"}
{_numfun |= "CEILING"}
{_numfun |= "FLOOR"}

{_positiveargfun |= "SQRT"}
{_positiveargfun |= "EXP"}

{_allnumfun |= "_numfun"}
{_allnumfun |= "_positiveargfun"}

{_stringfun |= ""}
-- There are no unary string-to-string functions supported yet.
--{_stringfun |= "LOWER"}
--{_stringfun |= "UPPER"}

{_genericagg |= "MIN"}
{_genericagg |= "MAX"}
{_genericagg |= "COUNT"}

{_numagg |= "SUM"}
{_numagg |= "AVG"}
{_numagg |= "_genericagg"}

{_maybe |= ""}
{_maybe |= " NOT "}

{_distinct |= ""}
{_distinct |= " DISTINCT "}

{_sortorder |= ""}
{_sortorder |= " ASC "}
{_sortorder |= " DESC "}

{_math |= " + "}
{_math |= " - "}
{_math |= " * "}
{_math |= " / "}
-- {_math |= " % "}

{_cmp |= "="}
{_cmp |= "<>"}
{_cmp |= "<"}
{_cmp |= ">"}
{_cmp |= "<="}
{_cmp |= ">="}
{_cmp |= "!="} -- Apparently, an HSQL-supported alias for the standard <>

{_jointype |= "INNER"}
{_jointype |= "LEFT"}
{_jointype |= "RIGHT"}

{_setop |= " UNION "}
{_setop |= " INTERSECT "}
{_setop |= " EXCEPT "}
{_setop |= " UNION ALL "}
{_setop |= " INTERSECT ALL "}
{_setop |= " EXCEPT ALL "}

{_logicop |= " AND "}
{_logicop |= " OR "}


{_inoneint     |= " _variable[numeric] IN (_value[int:1,100]) "}
{_inpairofints |= " _variable[numeric] IN (_value[int:101,199], _value[int:201,299]) "}
{_insomefixedints   |= " _variable[numeric] IN (1, 100, 1000, 10000, 100000, 1000000, 10000000, -1, -100, -1000, -10000, -100000, -1000000, -10000000) "}

{_inints |= "_inoneint"}
{_inints |= "_inpairofints"}
--{_inints |= "_insomefixedints"}

{_inonestring     |= " _variable[string] IN (_value[string]) "}
{_inpairofstrings |= " _variable[string] IN (_value[string], _value[string]) "}
{_insomefixedstrings   |= " _variable[string] IN ('1', '100', '1000', '10000', '100000', '1000000', '10000000', '-1', '-100', '-1000', '-10000', '-100000', '-1000000', '-10000000') "}

{_instrings |= "_inonestring"}
{_instrings |= "_inpairofstrings"}
--{_instrings |= "_insomefixedstrings"}

{_numericvalue |= "_value[int]"}
--HSQL and VoltDB disagree about how to make mixed types agree when comparing int columns
-- to float/decimal constants.
--{_numericvalue |= "_value[float]"}
--{_numericvalue |= "_value[decimal]"}

{_numericcolumnpredicate |= "_variable[numeric] _cmp _numericvalue"}
--{_numericcolumnpredicate |= "_inints"}

{_numericcolumnpredicatepair |= "( (_maybe _numericcolumnpredicate) _logicop (_maybe _numericcolumnpredicate) )"}

{_numericcolumnpredicatepairorless |= "_numericcolumnpredicate"}
{_numericcolumnpredicatepairorless |= "_numericcolumnpredicatepair"}

{_numericcolumnpredicatetriple |= "( _numericcolumnpredicatepair )  _logicop (_maybe _numericcolumnpredicate) )"}


{_stringcolumnpredicate |= "_variable[string] _cmp _value[string]"}
{_stringcolumnpredicate |= "SUBSTRING ( _variable[string] FROM _value[int:1,10] FOR _value[int:1,10] ) _cmp _value[string]"}
--{_stringcolumnpredicate |= "_instrings"}

{_stringcolumnpredicatepair |= "( (_maybe _stringcolumnpredicate) _logicop (_maybe _stringcolumnpredicate) )"}

{_stringcolumnpredicatepairorless |= "_stringcolumnpredicate"}
{_stringcolumnpredicatepairorless |= "_stringcolumnpredicatepair"}

{_stringcolumnpredicatetriple |= "( _stringcolumnpredicatepair )  _logicop (_maybe _stringcolumnpredicate) )"}

--Until we support ad hoc construction of timestamp constants.
--{_timestampcolumnpredicate |= "_variable[timestamp] _cmp _value[timestamp]"}
{_timestampcolumnpredicate |= "EXTRACT ( SECOND FROM      _variable[#reusable timestamp] ) = 1"}
{_timestampcolumnpredicate |= "EXTRACT ( MINUTE FROM      _variable[#reusable timestamp] ) = 1"}
{_timestampcolumnpredicate |= "EXTRACT ( HOUR FROM        _variable[#reusable timestamp] ) = 1"}
{_timestampcolumnpredicate |= "EXTRACT ( DAY FROM         _variable[#reusable timestamp] ) = 1"}
{_timestampcolumnpredicate |= "EXTRACT ( MONTH FROM       _variable[#reusable timestamp] ) = 1"}
{_timestampcolumnpredicate |= "EXTRACT ( QUARTER FROM     _variable[#reusable timestamp] ) = 1"}
{_timestampcolumnpredicate |= "EXTRACT ( YEAR FROM        _variable[#reusable timestamp] ) = 2013"}
{_timestampcolumnpredicate |= "EXTRACT ( DAY_OF_WEEK FROM _variable[#reusable timestamp] ) = 1"}
{_timestampcolumnpredicate |= "EXTRACT ( DAY_OF_YEAR FROM _variable[#reusable timestamp] ) = 1"}
-- Not quite implemented in front end?
--{_timestampcolumnpredicate |= "EXTRACT ( WEEK_OF_YEAR FROM _variable[#reusable timestamp] ) = 1"}

{_timestampcolumnpredicatepair |= "( (_maybe _timestampcolumnpredicate) _logicop (_maybe _timestampcolumnpredicate) )"}

{_timestampcolumnpredicatepairorless |= "_timestampcolumnpredicate"}
{_timestampcolumnpredicatepairorless |= "_timestampcolumnpredicatepair"}

{_timestampcolumnpredicatetriple |= "( _timestampcolumnpredicatepair )  _logicop (_maybe _timestampcolumnpredicate) )"}

