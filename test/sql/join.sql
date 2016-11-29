select ipaddress, firstname, lastname, gender, gender_desc from user
join gender_dim on  gender = gender_code;
