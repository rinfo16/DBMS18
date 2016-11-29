select firstname, lastname, gender 
from user
where gender = 1;

select firstname, lastname, ipaddress 
from user
where ipaddress = '49.114.31.119';

select id, firstname, lastname
from user
where id < 50;

select id, firstname, lastname
from user
where id > 951;

select id, firstname, lastname
from user
where id <= 50;

select id, firstname, lastname
from user
where id >= 951;

select id, firstname, lastname, gender 
from user
where gender = 1 and id <= 30;

select id, firstname, lastname, gender 
from user
where gender = 1 or id <= 30;

select id, firstname, lastname, gender 
from user
where gender = 1 and id >= 10 and id <= 30;

