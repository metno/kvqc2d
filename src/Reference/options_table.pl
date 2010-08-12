#!/usr/bin/perl
#
open (FILE,"ReadProgramOptions.cc");
while (<FILE>) {
	          chomp($_);
                  if($_=~m/DOCME/){
                     $_=~s/ *\(//;
                     $_=~s/\) *\/\/DOCME$//;
                     $_=~s/  //;
                     $_=~s/  //g;
                     $_=~s/"//g;
                     @P = split(/,/, $_);
                     #print $P[1] "\n";
                     print "|$P[0]|$P[1]|$P[2]|\n";
                  }
}
close(FILE);
