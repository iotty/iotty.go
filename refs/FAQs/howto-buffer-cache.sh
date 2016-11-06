# Linux buffer/cache HOWTO

dd if=/dev/zero of=hugefile bs=2M count=1000

# drop caches
echo 3 | tee /proc/sys/vm/drop_caches

free -m

time cat hugefile > /dev/null

free -m

time cat hugefile > /dev/null

rm -f hugefile
