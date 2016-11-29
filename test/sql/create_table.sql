create table user (
	id INTEGER, 
	firstname VARCHAR(256), 
	lastname VARCHAR(256) , 
	email VARCHAR(256), 
	gender INTEGER, 
	ipaddress VARCHAR(256)
);

create table gender_dim (
  gender_code INTEGER, 
  gender_desc VARCHAR(10)
);
