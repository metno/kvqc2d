#!/usr/bin/perl

# needs libdbd-pg-perl
# see http://stinfosys/veil1.html
# run with " PGPASSWORD=... perl stinfosys-vipp-pluviometer.pl" (space at beginning to avoid bash history)

use DBI;
use strict;

my $conn = DBI->connect("dbi:Pg:dbname=stinfosys;host=stinfosys;port=5435", "pstinfosys");

my %resolution_stations = ();

my $query = $conn->prepare(
    "SELECT si.stationid, em.description FROM sensor_info AS si, equipment AS eq, equipmentmodel AS em "
    . "WHERE em.description LIKE '%mm per vipp%' "
    . "AND paramid = 105 "
    . "AND em.modelname = eq.modelname AND si.equipmentid = eq.equipmentid "
    . "AND ((em.fromtime ISNULL) OR (em.totime ISNULL AND em.fromtime <= now()) "
        . "     OR (em.totime NOTNULL AND now() BETWEEN em.fromtime AND em.totime)) "
    . "ORDER BY si.stationid"
    );
$query->execute();

my (@row);
while (@row = $query->fetchrow_array()) {
    my ($stationid, $description)= @row;
    $description =~ /(^|\|)(\d+(\.\d+))mm per vipp/;
    my $vipp = $2;
    if( exists $resolution_stations{ $vipp } ) {
        push( @{$resolution_stations{ $vipp }}, $stationid );
    } else {
        $resolution_stations{ $vipp } = [$stationid];
    }
}

my $stations_par = "";
my $stsep = "";
for my $vipp (sort(keys %resolution_stations)) {
    $stations_par .= $stsep . $vipp . ":" . join(',', @{$resolution_stations{$vipp}});
    $stsep .= ";";
}

print "# station list extracted on " . gmtime() . " UTC\n";
print "stations = " . $stations_par . "\n";

$conn->disconnect();
$conn = undef;
