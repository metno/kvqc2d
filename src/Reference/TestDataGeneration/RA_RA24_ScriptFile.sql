
UPDATE data SET original=((SELECT SUM(original) FROM data WHERE obstime BETWEEN date '2027-06-13 06:00:00' - interval '3 days' AND '2027-06-13 06:00:00' AND stationid=57390 AND typeid=302 AND paramid=110 AND original>-32767)+(SELECT COUNT(original) FROM data WHERE obstime BETWEEN date '2027-06-13 06:00:00' - interval '3 days' AND '2027-06-13 06:00:00' AND stationid=57390 AND typeid=302 AND paramid=110 AND original=-1)) WHERE obstime='2027-06-13 06:00:00' AND stationid=57390 AND typeid=302 AND paramid=110 AND original>-32767 AND (SELECT COUNT(original) FROM data WHERE obstime BETWEEN date '2027-06-13 06:00:00' - interval '3 days' AND '2027-06-13 06:00:00' AND stationid=57390 AND typeid=302 AND paramid=110 AND original>-32767)=4;

UPDATE data SET controlinfo='1140000000002000' WHERE obstime='2027-06-13 06:00:00' AND stationid=57390 AND typeid=302 AND paramid=110 AND original>-32767 AND (SELECT COUNT(original) FROM data WHERE obstime BETWEEN date '2027-06-13 06:00:00' - interval '3 days' AND '2027-06-13 06:00:00' AND stationid=57390 AND typeid=302 AND paramid=110 AND original>-32767)=4;
UPDATE data SET controlinfo='1110000000002000' WHERE obstime BETWEEN date '2027-06-13 06:00:00' -interval '1 days' AND date '2027-06-13 06:00:00' -interval '0 days' AND stationid=57390 AND typeid=302 AND paramid=110 AND original>-32767 AND (SELECT COUNT(original) FROM data WHERE obstime BETWEEN date '2027-06-13 06:00:00' - interval '3 days' AND '2027-06-13 06:00:00' AND stationid=57390 AND typeid=302 AND paramid=110 AND original>-32767)=4;

UPDATE data SET original=-11111 WHERE obstime BETWEEN date '2027-06-13 06:00:00' -interval '1 days' AND date '2027-06-13 06:00:00' -interval '0 days' AND stationid=57390 AND typeid=302 AND paramid=110 AND original>-32767 AND (SELECT COUNT(original) FROM data WHERE obstime BETWEEN date '2027-06-13 06:00:00' - interval '3 days' AND '2027-06-13 06:00:00' AND stationid=57390 AND typeid=302 AND paramid=110 AND original>-32767)=4;

UPDATE data SET controlinfo='1110000000002000' WHERE obstime BETWEEN date '2027-06-13 06:00:00' -interval '2 days' AND date '2027-06-13 06:00:00' -interval '1 days' AND stationid=57390 AND typeid=302 AND paramid=110 AND original>-32767 AND (SELECT COUNT(original) FROM data WHERE obstime BETWEEN date '2027-06-13 06:00:00' - interval '3 days' AND '2027-06-13 06:00:00' AND stationid=57390 AND typeid=302 AND paramid=110 AND original>-32767)=4;

UPDATE data SET original=-11111 WHERE obstime BETWEEN date '2027-06-13 06:00:00' -interval '2 days' AND date '2027-06-13 06:00:00' -interval '1 days' AND stationid=57390 AND typeid=302 AND paramid=110 AND original>-32767 AND (SELECT COUNT(original) FROM data WHERE obstime BETWEEN date '2027-06-13 06:00:00' - interval '3 days' AND '2027-06-13 06:00:00' AND stationid=57390 AND typeid=302 AND paramid=110 AND original>-32767)=4;

UPDATE data SET controlinfo='1110000000002000' WHERE obstime BETWEEN date '2027-06-13 06:00:00' -interval '3 days' AND date '2027-06-13 06:00:00' -interval '2 days' AND stationid=57390 AND typeid=302 AND paramid=110 AND original>-32767 AND (SELECT COUNT(original) FROM data WHERE obstime BETWEEN date '2027-06-13 06:00:00' - interval '3 days' AND '2027-06-13 06:00:00' AND stationid=57390 AND typeid=302 AND paramid=110 AND original>-32767)=4;

UPDATE data SET original=-11111 WHERE obstime BETWEEN date '2027-06-13 06:00:00' -interval '3 days' AND date '2027-06-13 06:00:00' -interval '2 days' AND stationid=57390 AND typeid=302 AND paramid=110 AND original>-32767 AND (SELECT COUNT(original) FROM data WHERE obstime BETWEEN date '2027-06-13 06:00:00' - interval '3 days' AND '2027-06-13 06:00:00' AND stationid=57390 AND typeid=302 AND paramid=110 AND original>-32767)=4;

UPDATE data SET cfailed=cfailed||'TEST-DATA-RA_RR24 original='||corrected, original=-32767 WHERE obstime BETWEEN date '2027-06-13 06:00:00' - interval '3 days' AND '2027-06-13 06:00:00' AND stationid=57390 AND typeid=302 AND paramid=110 AND original=-11111;

UPDATE data SET cfailed=cfailed||'TEST-DATA-RA_RR24 original='||corrected WHERE obstime='2027-06-13 06:00:00' AND stationid=57390 AND typeid=302 AND paramid=110;
