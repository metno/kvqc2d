QC2 statistical checks
======================

Configuration
-------------

 * `tolerance`, default 10
 * `days`, default 30
 * `days_required`, default = 90% of days
 * `InterpolationDistance`, max distance to neighbors in km, default 100

There is no configuration to limit the station ids -- they are implicitly limited
via the table `qc2_statistical_reference_values`.

For example:

        Start_YYYY = 2012
        Start_MM   =    2
        Start_DD   =    1
        Start_hh   =   06
        End_YYYY   = 2012
        End_MM     =    2
        End_DD     =    7
        days       =   30
        tolerance  =    1
        ParamId    =  109
        TypeIds    =  330
        TypeIds    =  342
        InterpolationDistance = 50

There are some examples in `../test/StatisticalMeanTest.cc`, but this
is not the easiest reading.

Parameters
----------

 * mean applies primarily to paramids 178 (PR) and 211 (TA)
 * sum applies peimarily to paramids 106 (RR_1), 108 (RR_6), 109 (RR_12), 110 (RR_24)
 * quartiles applies primarily to paramids 262 (UU), 15 (NN), 55 (HL), 273 (VV), 2070 (QSI)

Reference values
----------------

Reference values are stored in table `qc2_statistical_reference_values` in kvalobs, with columns

 * `stationid` INTEGER
 * `paramid` INTEGER
 * `day_of_year` INTEGER, 1..365 where Feb 29 is treated as Feb 28
 * `key` TEXT, which is
   * `ref_q1`, `ref_q2`, `ref_q3` for quartiles
   * `ref_value` for mean and sum
 * `value` FLOAT

Output
------

When a test finds suspicious data, it prints WARN messages like

    statistical test triggered for <center stationid> for series ending at <date>
