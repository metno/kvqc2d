#!/usr/bin/perl

# needs libdbd-pg-perl
# see http://stinfosys/veil1.html
# run with " PGPASSWORD=... perl stinfosys-vipp.pl" (space at beginning to avoid bash history)

use DBI;
use strict;

my $conn = DBI->connect("dbi:Pg:dbname=stinfosys;host=stinfosys;port=5435", "pstinfosys");

sub stationsWithResolution {
    my ($stationid, $vipp, $infolist, $sep, @row);
    $vipp = sprintf("%.1f", $_[0]);
    my $query = $conn->prepare(
        "SELECT si.stationid FROM sensor_info AS si, equipment AS eq, equipmentmodel AS em "
        . "WHERE em.description LIKE '%" . $vipp . "mm per vipp%' "
        . "AND paramid = 105 "
        . "AND em.modelname = eq.modelname AND si.equipmentid = eq.equipmentid "
        . "AND ((em.fromtime ISNULL) OR (em.totime ISNULL AND em.fromtime <= now()) "
        . "     OR (em.totime NOTNULL AND now() BETWEEN em.fromtime AND em.totime)) "
        . "ORDER BY si.stationid"
        );
    $query->execute();

    $infolist = $vipp;
    $sep=":";
    while (@row = $query->fetchrow_array()) {
        ($stationid) = @row;
        $infolist .= $sep . sprintf("%d", $stationid);
        $sep = ",";
    }
    undef($query);
    return $infolist;
}

my $stations = "stations =";
my $sep=" ";
foreach my $mmpv ( (0.1, 0.2) ) {
    $stations .= $sep . stationsWithResolution($mmpv);
    $sep=";";
}
print $stations, "\n";

$conn->disconnect();
$conn = undef;
