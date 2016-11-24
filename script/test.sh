cur_dir=$(pwd)
script_dir=$(cd `dirname $0`; pwd)
cd $script_dir
cd ../
project_path=$(pwd)
cd $cur_dir
cd $project_path/test/sql

data_dir=$project_path/test/data

psql -h localhost -p 8432 -d postgres -f create_table.sql
psql -h localhost -p 8432 -d postgres -f copy_from_std.sql
psql -h localhost -p 8432 -d postgres -f copy_to_std.sql
psql -h localhost -p 8432 -d postgres -f select.sql
