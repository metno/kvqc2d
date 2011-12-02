#!/usr/bin/perl

use strict;
use MIME::Lite;
use POSIX qw(strftime);
use Sys::Hostname;

# first argument: logfile basename
my $logfile = "/var/log/kvalobs/kvqc2d.log";
if( $#ARGV >= 0 ) {
    $logfile = $ARGV[0];
}

# second argument: logname
my $logname;
if( $#ARGV >= 1 ) {
    $logname = $ARGV[1];
} elsif( $logfile =~ /(^|\/)([a-z_aZ_0-9-]+)\.log$/ ) {
    $logname = $2;
} else {
    $logname = "unknown";
}

#third argument: how many logfiles to check
my $maxlogs = 2;
if( $#ARGV >= 2 ) {
    $maxlogs = $ARGV[2];
}

my $sincefile = "$logfile.since";
my %colors = ( "WARN" => "#FFBA00", "ERROR" => "#FF3800", "FATAL" => "#FF00A0" );
#my $recipients = 'bjorn.nordin@met.no, oysteinl@met.no, pal.sannes@met.no, alexander.buerger@met.no';
my $recipients = 'alexander.buerger@met.no';

########################################################################

# get timestamp from logfile extraction, if it exists, or return ""
sub read_since {
    my $SINCE;
    my $s = "";
    if( open SINCE, "$sincefile" ) {
        my @lines = <SINCE>;
        $s = $lines[0];
        close SINCE;
    }
    return $s;
}

# write timestamp for next logfile extraction
sub write_since {
    my ($n) = @_;
    if( open SINCE, ">$sincefile" ) {
        print SINCE $n;
        close SINCE;
    }
}

sub logtable_head {
    my ($logname, $now, $since) = @_;
    my $h = "<html>\n"
        . "  <body style=\"font-family:'sans-serif'; fons-size: small;\">\n"
        . "    <h1>$logname/" . hostname ." logfile extract</h1>\n"
        . "    Extracted from $logfile* on " . hostname . " at $now";
    if( $since gt "2010" ) {
        $h .= " for logfile entries since $since";
    }
    $h  .= "\n";
    return $h;
}

sub logtable_file {
    my ($filename, $since) = @_;
    my $html = "";
    my $LOG;
    if( open LOG, $filename ) {
        my $message = undef;
        my $time = undef;
        my $level = undef;
        while (my $line = <LOG>) {
            if( $line =~ /(20\d\d-\d\d-\d\d \d\d:\d\d:\d\d) ([A-Z]+)/ ) {
                if( ($time gt $since) && $message && $colors{$level} ) {
                    $html .= "      <tr>"
                        . "<td valign=\"top\" nowrap>$time</td>"
                        . "<td valign=\"top\" bgcolor=\"" . $colors{$level} . "\">$level</td>"
                        . "<td>$message</td>"
                        . "</tr>\n";
                }
                $message = "";
                $time = $1;
                $level = $2;
            } elsif( $line =~ /^-{20} (.*)$/ ) {
                $message .= $1;
            } else {
                $message .= $line;
            }
        }
        close LOG;
    }
    return $html;
}

sub logtable_files {
    my ($filebase, $since) = @_;
    my $html = "";
    for (my $count = $maxlogs; $count >= 1; $count--) {
        $html .= logtable_file("$filebase.$count", $since);
    }
    $html .= logtable_file("$filebase", $since);
    if( $html ) {
        $html = "    <table rules=\"all\" border=\"2\" cellspacing\"2\" cellpadding=\"2\">\n"
            . "      <thead><tr><th>Date-Time</th><th>Level</th><th align=\"left\">Message</th></tr></thead>\n"
            . $html
            . "    </table>\n";
    } else {
        $html = "<p>No log entries in this period.</p>\n";
    }
    return $html;
}

sub logtable_tail
{
    return "  </body>\n"
        . "</html>\n";
}

########################################################################

my $since = read_since;
my $now = strftime "%Y-%m-%d %H:%M:%S", gmtime;
my $html = logtable_head($logname, $now, $since)
    . logtable_files($logfile, $since)
    . logtable_tail;

if( $#ARGV < 3 ) {
    my $email = MIME::Lite->new(
        To      => $recipients,
        From    => 'kvdrift@met.no',
        Subject => ($logname . '/' . hostname . ' logfile extract'),
        Type    => 'text/html',
        Data    => $html
        );
    $email->send();
} else {
    print $html;
}

write_since($now);
