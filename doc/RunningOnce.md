Running once
------------

Since version 1.5.16 of kvqc2d, it is possible to run one specific
check from the command line. It uses the same configuration file
format as when running as daemon, but neither `RunAtHour` nor
`RunAtMinute` are used.

If the `kvqc2d` daemon is running, you **must** stop it before running a single check:

    kvstop kvqc2d

Then, run one check:

    kvqc2d --logfile one.log --run-config one.cfg2

The option `--run-config one.cfg2` specifies the configuration file for the check.

The option `--logfile one.log` specifies that all log messages should be written to the given file.
There is no limit on the file size.
This option also disables writing of log messages to `/var/log/kvalobs/kvqc2d.log`.

If the `kvqc2d` daemon was running, you **should** start it again:

    kvstart kvqc2d

To analyze the log file, you may want to use the `kvalobs-log-mailer` program like this:

    kvalobs-log-mailer --logfile=one.log --hqc-only --debug-no-mail > one.html

This will read the logfile, format the messages, and the output will be written to the specified HTML file.
