#
# If required to run only on a Wednesday then place this in the crontab file:
#
# 23 0 * * 3 /etc/kvalobs/Qc2Config/Redistribution_14days_on_cron.sh
# 23 0 * * 4 /etc/kvalobs/Qc2Config/Redistribution_14days_off_cron.sh
#
# and check that the scripts are set to executable
#
/bin/mv /etc/kvalobs/Qc2Config/Redistribution_14days.hold /etc/kvalobs/Qc2Config/Redistribution_14days.cfg
