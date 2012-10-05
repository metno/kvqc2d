# crontab entry for kvqc2d

SHELL=/bin/sh
PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin

# m h  dom mon dow   user command
0 3 * * * kvalobs /usr/lib/kvqc2d/plu-autoupdate-from-stinfosys
