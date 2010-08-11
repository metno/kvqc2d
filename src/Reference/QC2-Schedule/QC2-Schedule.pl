#!/usr/bin/perl
#
#  Simple Perl script to plot the start times of each Qc2 algorithm 
#  paule@met.no 2010-10-02
#
#  Usage: 
#
#    ./QC2-Schedule.pl input-directory [output-filename]
#
#    input-directory : the path where the configuration files are located
#    e.g. /etc/kvalobs/Qc2Config
#
#    output-filename : an OPTIONAL filename for the output. The output is in html
#    so a filename of the form XXX.html is advised. If no filename is given
#    the script will write (overwrite mode) to QC2-Schedule.html in the local directory.
#
#    NB: Change the value of $ip when installing this file on a new system and place the
#    files:
#      bluebar.gif  blue-line.gif  busy.gif  shim.gif 
#    in the corresponding directory.
#
# Based on an original concept for Gant charts in html by Joe Lynds 2002-2008  http://www.jlion.com
#
$ip="/home/paule/PM/";

# Argument check and usage statement

if ($#ARGV < 0){
   print("Usage is : $0 input_directory [output_file.html]\n");
   exit
}


opendir(IMD, "$ARGV[0]") || die("Cannot open directory"); 

if ($#ARGV == 1){
  open(OUTFILE, ">$ARGV[1]") || die("Cannot open output file");
} else {
  open(OUTFILE, "> QC2-Schedule.html") || die("Cannot open output file");
}

my @files = grep {/.*\.cfg/} readdir IMD;
close IMD;

# Print preamble
&print_preamble(OUTFILE);

foreach my $f (@files) {

					  open (FILE,"$ARGV[0]/$f");
					  while (<FILE>) {
					     chomp($_);
                         if($_=~m/RunAtHour/){
							     $_=~s/.*=//;
							     $HOUR=$_;
							     }
                         if($_=~m/RunAtMinute/){
							     $_=~s/.*=//;
							     $MIN=$_;
							     }
                         if($_=~m/ParamId/){
							     $_=~s/.*=//;
							     $PAR=$_;
							     }
					     $MULT=$MIN*$HOUR;
					     $OFFSET=$MIN*0.5;
						 $COLSET=23-$HOUR;
						 if ($HOUR==23) {$COLSET=0};
						 if ($HOUR==0)  {$COLSET=23};
   
					  }
                      print OUTFILE "<tr>\n";
                      print OUTFILE "<td>$f</td>\n";
   
                      if ($HOUR>0 && $HOUR<23) {
                         print OUTFILE "<td  colspan=$HOUR></td>\n";
                         print OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/shim.gif\" height=\"15px;\" width=\"($OFFSET)px;\"><img src=\"$ip/bluebar.gif\" height=\"15px;\" width=\"2px;\"></td>\n";
                         print OUTFILE "<td  colspan=$COLSET></td>\n";
                         print OUTFILE "<td align=\"right\">$PAR</td>\n";
                         print OUTFILE "</tr>\n";
                      } 
					  if ($HOUR==0) {
                         print OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/shim.gif\" height=\"15px;\" width=\"($OFFSET)px;\"><img src=\"$ip/bluebar.gif\" height=\"15px;\" width=\"2px;\"></td>\n";
                         print OUTFILE "<td  colspan=23></td>\n";
                         print OUTFILE "<td align=\"right\">$PAR</td>\n";
                         print OUTFILE "</tr>\n";
                      }
					  if ($HOUR==23) {
                         print OUTFILE "<td  colspan=$HOUR></td>\n";
                         print OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/shim.gif\" height=\"15px;\" width=\"($OFFSET)px;\"><img src=\"$ip/bluebar.gif\" height=\"15px;\" width=\"2px;\"></td>\n";
                         print OUTFILE "<td align=\"right\">$PAR</td>\n";
                         print OUTFILE "</tr>\n";
                      }
					  close(FILE);
}
&print_end(OUTFILE);	
close(OUTFILE);

###########################################
# Subroutine containing static html code. #
###########################################

sub print_preamble {

$OUTFILE=$_[0];

print $OUTFILE "<html>\n";
print $OUTFILE "<head>\n";
print $OUTFILE "<style type=\"text/css\">\n";
	print $OUTFILE "td { font-family: arial; font-size: 10px; }\n";
print $OUTFILE "</style>\n";
print $OUTFILE "</head>\n";
print $OUTFILE "<BODY>\n";

print $OUTFILE "<table rules=\"rows\"> \n";
print $OUTFILE "<tr>\n";
	print $OUTFILE "<td  style=\"width: 500px;\">Configuration Filename</td>\n";
	print $OUTFILE "<td style=\"width: 756px;\" colspan=\"24\">Hour (UT)</td>\n";
    print $OUTFILE "<td>ParamID</td>\n";
print $OUTFILE "</tr>\n";

print $OUTFILE "<tr>\n";
	print $OUTFILE "<td>...</td>\n";
	print $OUTFILE "<td align=\"center\">0</td>\n";
	print $OUTFILE "<td align=\"center\">1</td>\n";
	print $OUTFILE "<td align=\"center\">2</td>\n";
	print $OUTFILE "<td align=\"center\">3</td>\n";
	print $OUTFILE "<td align=\"center\">4</td>\n";
	print $OUTFILE "<td align=\"center\">5</td>\n";
	print $OUTFILE "<td align=\"center\">6</td>\n";
	print $OUTFILE "<td align=\"center\">7</td>\n";
	print $OUTFILE "<td align=\"center\">8</td>\n";
	print $OUTFILE "<td align=\"center\">9</td>\n";
	print $OUTFILE "<td align=\"center\">10</td>\n";
	print $OUTFILE "<td align=\"center\">11</td>\n";
	print $OUTFILE "<td align=\"center\">12</td>\n";
	print $OUTFILE "<td align=\"center\">13</td>\n";
	print $OUTFILE "<td align=\"center\">14</td>\n";
	print $OUTFILE "<td align=\"center\">15</td>\n";
	print $OUTFILE "<td align=\"center\">16</td>\n";
	print $OUTFILE "<td align=\"center\">17</td>\n";
	print $OUTFILE "<td align=\"center\">18</td>\n";
	print $OUTFILE "<td align=\"center\">19</td>\n";
	print $OUTFILE "<td align=\"center\">20</td>\n";
	print $OUTFILE "<td align=\"center\">21</td>\n";
	print $OUTFILE "<td align=\"center\">22</td>\n";
	print $OUTFILE "<td align=\"center\">23</td>\n";
    print $OUTFILE "<td align=\"right\">...</td>\n";
print $OUTFILE "</tr>\n";

print $OUTFILE "<tr>\n";
    print $OUTFILE "<td>...</td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
print $OUTFILE "<td align=\"right\">...</td>\n";
print $OUTFILE "</tr>\n";

}

################################################
# Subroutine containing more static html code. #
################################################

sub print_end {
$OUTFILE=$_[0];

   print $OUTFILE "<tr>\n";
    print $OUTFILE "<td>...</td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
    print $OUTFILE "<td style=\"width: 31.5px;\"><img src=\"$ip/busy.gif\" height=\"15px;\" width=\"31.5px;\"></td>\n";
   print $OUTFILE "<td align=\"right\">...</td>\n";
   print $OUTFILE "</tr>\n";

   print $OUTFILE "<tr>\n";
	print $OUTFILE "<td>...</td>\n";
	print $OUTFILE "<td align=\"center\">0</td>\n";
	print $OUTFILE "<td align=\"center\">1</td>\n";
	print $OUTFILE "<td align=\"center\">2</td>\n";
	print $OUTFILE "<td align=\"center\">3</td>\n";
	print $OUTFILE "<td align=\"center\">4</td>\n";
	print $OUTFILE "<td align=\"center\">5</td>\n";
	print $OUTFILE "<td align=\"center\">6</td>\n";
	print $OUTFILE "<td align=\"center\">7</td>\n";
	print $OUTFILE "<td align=\"center\">8</td>\n";
	print $OUTFILE "<td align=\"center\">9</td>\n";
	print $OUTFILE "<td align=\"center\">10</td>\n";
	print $OUTFILE "<td align=\"center\">11</td>\n";
	print $OUTFILE "<td align=\"center\">12</td>\n";
	print $OUTFILE "<td align=\"center\">13</td>\n";
	print $OUTFILE "<td align=\"center\">14</td>\n";
	print $OUTFILE "<td align=\"center\">15</td>\n";
	print $OUTFILE "<td align=\"center\">16</td>\n";
	print $OUTFILE "<td align=\"center\">17</td>\n";
	print $OUTFILE "<td align=\"center\">18</td>\n";
	print $OUTFILE "<td align=\"center\">19</td>\n";
	print $OUTFILE "<td align=\"center\">20</td>\n";
	print $OUTFILE "<td align=\"center\">21</td>\n";
	print $OUTFILE "<td align=\"center\">22</td>\n";
	print $OUTFILE "<td align=\"center\">23</td>\n";
    print $OUTFILE "<td align=\"right\">...</td>\n";
   print $OUTFILE "</tr>\n";

   print $OUTFILE "</table> \n";
   print $OUTFILE "</BODY> \n";
   print $OUTFILE "</HTML> \n";
}

