<?php


$db = mysql_connect('localhost', 'celinz1_uzyt1', 'placekjacek1');

echo $db . "<br>";

echo mysql_select_db('celinz1_baza1', $db);

echo "<br>";


$a = (int) $_GET[wynik];

$pytanie = "  
	INSERT INTO
		MAL_test1
	SET
		MAL_test1_wynik = {$a}";

echo mysql_query($pytanie);



echo "<br>OK";






?>