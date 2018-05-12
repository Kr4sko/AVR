<?php


$db = mysql_connect('mysql.cba.pl', 'celinz1_uzyt1', 'placekjacek1');
	
	if(!$db)
	{
		echo "ERROR";
		die;
	}	
//echo $db . "<br>";

//$ctdb = mysql_select_db('celinz1_uzyt1', $db);
	
	if(mysql_select_db('celinz1_uzyt1', $db)== FALSE)
	{
		echo "COULD NOT CONNECT TO DATABASE";
		die;
	}
	
//echo "<br>";


$a = (int) $_GET[wynik];

$pytanie = "  
	INSERT INTO
		MAL_test1
	SET
		MAL_test1_wynik = {$a}";

echo mysql_query($pytanie);


echo "<br>OK";






?>