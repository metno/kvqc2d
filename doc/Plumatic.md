Plumatic Check Configuration
============================

The *Plumatic* check for pluviometers is controlled by a file
`/etc/kvalobs/Qc2Config/Plumatic.cfg2`. The file contents decide
what to run and which options to use, for this check it must say

    Algorithm=Plumatic

The filename is actually not important as long as it ends with `.cfg2`
and .


Running once
------------

See the general documentation on [running checks once](RunningOnce.md).

You might want to adjust some options, for example

    # choose pluviometer check
    Algorithm=Plumatic
    
    ...
    
    # year, month, day, hour for start and end of the time range to check
    Start_YYYY = 2001
    Start_MM = 1
    Start_DD = 1
    Start_hh = 0
    End_YYYY = 2002
    End_MM = 1
    End_DD = 1
    End_hh = 0
    
    # mm-per-tipping:stationnumber,stationnumber;mm-per-tipping:stationnumber,...
    stations = 0.1:3015,3019,3030;0.2:3290,3370,4080
    
    # minutes<mm-precipitation;minutes<mm-precipitation;...
    sliding_alarms = 2<8.1;3<11.9
    
    ParamId = 105

    # without any TypeId = .. line, typeids 4 and 504 are checked
    TypeId = 1234
    TypeId = 3456

    vipps_unlikely_single = ...
    vipps_unlikely_start =  ...

Running Regularly
-----------------

This is the typical setup for production. The option `Last_NDays`
specifies how many days backwards should be checked, and there is no
configuration `Start_YYYY` and so on.

At MET Norway, the configuration `stations` is updated daily using a
cron job.

    Algorithm=Plumatic
    RunAtHour   =22
    RunAtMinute =12
    Last_NDays  = 4
    
    # plu-autoupdate-from-stinfosys
    # station list extracted on Mon Feb 22 03:00:04 2016 UTC
    stations = 0.1:123,234;0.2:345,456
    
    sliding_alarms = 2<8...
    ParamId = 105
    
    vipps_unlikely_single = ...
    vipps_unlikely_start =  ...
    vipps_rain_interrupt = ...
    rain_interrupt_max = ...
    rain_interrupt_before_after = ...
