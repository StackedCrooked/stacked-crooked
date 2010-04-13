#################
# Create the CA #
#################

# Create folder stucture and default files
mkdir -p CA/private
mkdir -p CA/newcerts
touch CA/index.txt
echo "00" >> CA/serial

# Create the CA 
openssl req -new -x509 -keyout ./CA/private/cakey.pem -out ./CA/cacert.pem -days 365 -passin pass:secret1 -passout pass:secret1 -config openssl.cnf


##############################
# Create a test certificate  #
##############################

# Create the request
openssl req -batch -config ./openssl.cnf -new -keyout test-certificate.key -out test-certificate.request -days 365 -passin pass:secret1 -passout pass:secret1 

# Create the certificate 
openssl ca -batch -config ./openssl.cnf -policy policy_anything -passin pass:secret1 -out test-certificate.pem -infiles test-certificate.request

# Cleanup request file
rm test-certificate.request

echo "test-certificate.pem has been generated."

##################################
# Sign a third party certificate #
##################################

# Create a key
openssl genrsa -des3 -passout pass:secret1 -out tmp.key 1024

# Remove the password from the key
cp tmp.key tmp.key.old
openssl rsa -in tmp.key.old -passin pass:secret1 -passout pass:secret1 -out tmp.key
rm tmp.key.old

# Sign the certificate with this key
openssl x509 -days 365 -in thirdparty.pem -signkey tmp.key -passin pass:secret1 -text -out thirdparty-signed.pem

# Remove tempfile
rm tmp.key


########################################
# Add the signed certificate to the CA #
########################################

# Create the certs dir if it doesn't already exist
mkdir -p CA/certs

# Copy our signed third party certificate
cp thirdparty-signed.pem CA/certs

# Create the hash-link
cd CA/certs
ln -s thirdparty-signed.pem `openssl x509 -hash -noout -in thirdparty-signed.pem`.0
cd -


#################################################
# Add the signed certificate to the CA database #
#################################################

openssl ca -config openssl.cnf -policy policy_anything -passin pass:secret1 -ss_cert thirdparty-signed.pem


####################
# Start the server #
####################

# Use test-certificate.pem as response signer.
openssl ocsp -CApath ./CA/certs -index CA/index.txt -port 5000 -rkey test-certificate.key -rsigner test-certificate.pem -CA CA/cacert.pem -text

# The client request:
# $ openssl ocsp -issuer CA/cacert.pem -VAfile test-certificate.pem -cert thirdparty.pem -url http://10.5.5.247:5000
# => Response verify OK
# => thirdparty.pem: unknown
#        This Update: Apr 13 13:00:59 2010 GMT

