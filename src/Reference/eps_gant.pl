#!/usr/bin/perl
use Time::Local;

# Argument check and usage statement

if ($#ARGV < 0){
   print("Usage is : $0 input_file(s)\n");
   exit
}

# Open files for input and process

open(LYNX,"cat @ARGV |");

while (<LYNX>) {

   $line=$_;
   if ($_=/FILE RECEIVED > OK/){
      if ($line=~m/...._..._.._..._..............Z_..............Z_._._..............Z/){
         $eps_prod=$&;
         push(@Products,$eps_prod);
         print "Product  : ",$eps_prod,"\n";
         }
      if ($line=~m/....._RPT_.._..._/){
         $eps_repo=$&;
         print "Report   : ",$eps_repo,"\n";
         }
      if ($line=~m/...._..._.._..._..............Z_..............Z_..............Z_...._........../){
         $eps_auxi=$&;
         print "Aux Data : ",$eps_auxi,"\n";
         }
   }

}

print "Number of eps products = ",$#Products+1,"\n";
# print $Products[0],"\n";
# print $Products[$#Products],"\n";

# Process the eps products ready for display in Gant type occurrence chart.

# Define Product types
@PTM01=("ADCS_xxx_00_M01",
        "HKTM_xxx_00_M01",
        "SEMx_xxx_00_M01",
        "AVHR_xxx_00_M01","AVHR_xxx_1A_M01","AVHR_xxx_1B_M01",
        "AMSA_xxx_00_M01","AMSA_xxx_1A_M01","AMSA_xxx_1B_M01",
        "HIRS_xxx_00_M01","HIRS_xxx_1A_M01","HIRS_xxx_1B_M01",
        "IASI_xxx_00_M01","IASI_VER_01_M01","IASI_ENG_01_M01",
                          "IASI_xxx_1A_M01","IASI_xxx_1B_M01","IASI_xxx_1C_M01",
        "MHSx_xxx_00_M01","MHSx_xxx_1A_M01","MHSx_xxx_1B_M01",
        "ASCA_xxx_00_M01","ASCA_xxx_1A_M01","ASCA_SZO_1B_M01","ASCA_SZR_1B_M01","ASCA_SZF_1B_M01",
        "GOME_xxx_00_M01","GOME_xxx_1A_M01","GOME_xxx_1B_M01",
        "GRAS_xxx_00_M01","GRAS_xxx_1A_M01","GRAS_xxx_1B_M01",
        "ATOV_SND_02_M01");
@PTM02=("ADCS_xxx_00_M02",
        "HKTM_xxx_00_M02",
        "SEMx_xxx_00_M02",
        "AVHR_xxx_00_M02","AVHR_xxx_1A_M02","AVHR_xxx_1B_M02",
        "AMSA_xxx_00_M02","AMSA_xxx_1A_M02","AMSA_xxx_1B_M02",
        "HIRS_xxx_00_M02","HIRS_xxx_1A_M02","HIRS_xxx_1B_M02",
        "IASI_xxx_00_M02","IASI_VER_01_M02","IASI_ENG_01_M02",
                          "IASI_xxx_1A_M02","IASI_xxx_1B_M02","IASI_xxx_1C_M02",
        "MHSx_xxx_00_M02","MHSx_xxx_1A_M02","MHSx_xxx_1B_M02",
        "ASCA_xxx_00_M02","ASCA_xxx_1A_M02","ASCA_SZO_1B_M02","ASCA_SZR_1B_M02","ASCA_SZF_1B_M02",
        "GOME_xxx_00_M02","GOME_xxx_1A_M02","GOME_xxx_1B_M02",
        "GRAS_xxx_00_M02","GRAS_xxx_1A_M02","GRAS_xxx_1B_M02",
        "ATOV_SND_02_M02");
@PTN18=("NOAA_GAC_00_N18",
        "HKTM_xxx_00_N18",
        "AVHR_GAC_00_N18","AVHR_GAC_1A_N18","AVHR_GAC_1B_N18",
        "AVHR_xxx_00_N18","AVHR_xxx_1A_N18","AVHR_xxx_1B_N18",
        "AMSA_xxx_00_N18","AMSA_xxx_1A_N18","AMSA_xxx_1B_N18",
        "HIRS_xxx_00_N18","HIRS_xxx_1A_N18","HIRS_xxx_1B_N18",
        "MHSx_xxx_00_N18","MHSx_xxx_1A_N18","MHSx_xxx_1B_N18",
        "ATOV_SND_02_N18");

#loop through the products
         print "Product Type ---  Start ---  Time Duration \n";

foreach $product (@Products) {

        
   if ($product=~m/M01/){
      &process_filename("M01",$product);
      }

   if ($product=~m/M02/){
      &process_filename("M02",$product);
      }

   if ($product=~m/N18/){
      &process_filename("N18",$product);
      }
}

sub process_filename {

   $filename=$_[1];
   $pt=$filename;
   $pt=~s/_..............Z_..............Z_._._..............Z//;
   @fields=split(/_/,$filename);
   #print $filename," ", $fields[0]," ", $fields[1]," ", $fields[2]," ", $fields[3]," ", $fields[4]," ", $fields[5]," ", $fields[6]," ", $fields[7]," ", $fields[8],"\n";
         $_=$fields[4];
         ($sa,$sb,$sm,$sd,$sh,$smi,$ss)=/../g;
         $syear=$sa.$sb; 
         $_=$fields[5];
         ($ea,$eb,$em,$ed,$eh,$emi,$es)=/../g;
         $eyear=$ea.$eb; 
  
         #print $ea," ",$eb," ",$em," ",$ed," ",$eh," ",$emi," ",$es,"\n";

         #$end_time = timelocal($es, $emi, $eh, $ed, $em, $eyear);
         $start_time = timegm($ss, $smi, $sh, $sd, $sm, $syear);
         $end_time = timegm($es, $emi, $eh, $ed, $em, $eyear);
         #print $end_time-$start_time,"\n";
         print $filename," ",$pt," ",$start_time," ",$end_time/60.0-$start_time/60.0,"\n";

             
}

# e.g.  NOAA_GAC_00_N18_20060117130045Z_20060117140612Z_N_O_20060117143923Z

