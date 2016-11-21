cur_dir=$(pwd)
script_dir=$(cd `dirname $0`; pwd)
cd $script_dir
cd ../
project_path=$(pwd)
cd $cur_dir
cd $project_path/build
./unit_test/cmd.test --csv --export --path=/tmp/data.csv --row=100
psql -h localhost -p 8432 -d postgres -c "create table product1 (id INTEGER, price INTEGER, name VARCHAR(256));"
psql -h localhost -p 8432 -d postgres -c "create table product2 (id INTEGER, price INTEGER, name VARCHAR(256));"
psql -h localhost -p 8432 -d postgres -c "copy product1 from '/tmp/data.csv';"
psql -h localhost -p 8432 -d postgres -c "copy product2 from '/tmp/data.csv';"
psql -h localhost -p 8432 -d postgres -c "select name, id, price from product1;"
psql -h localhost -p 8432 -d postgres -c "select name, id, price from product2;"
psql -h localhost -p 8432 -d postgres -c "select product1.name, product2.id, product2.price from product1 join product2 on product1.id = product2.id;"
