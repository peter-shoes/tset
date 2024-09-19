total_tests=0
passed_tests=0

for filename in $1/*.t; do
  base=$(basename $filename)
	./bin/tset $filename /tmp/"${base}"ex
  if diff "${filename}"ex /tmp/"${base}"ex > /dev/null; then
    echo "\e[32mTest passed\e[0m: $base"
    passed_tests=$((passed_tests+1))
  else
    echo "\e[31mTest failed\e[0m: $base"
  fi
  total_tests=$((total_tests+1))
done

echo "[$passed_tests/$total_tests] passed"
