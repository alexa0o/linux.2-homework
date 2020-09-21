sudo rmmod phonebook_module
make
make test

echo "0 Surname Name Age Email Phone" > /dev/phonebook
echo "1 Surname" > /dev/phonebook
cat /dev/phonebook
echo
echo "2 Surname" > /dev/phonebook
echo "1 Surname" > /dev/phonebook
cat /dev/phonebook
echo

echo "0 Surname Name Age Email Phone" > /dev/phonebook
echo "0 Supersurname Supername Superage Superemail Superphone" > /dev/phonebook
echo "0 Vladimirov Vladimir 27 vl@vl.com 8-911-133-74-88" > /dev/phonebook
echo "1 Vladimirov" > /dev/phonebook
cat /dev/phonebook
echo
echo "1 Supersurname" > /dev/phonebook
cat /dev/phonebook
echo
