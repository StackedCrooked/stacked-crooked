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





#########################################
# Sign the third-party root certificate #
#########################################

# Sign Adobe root certificate with our CA private key
openssl ca -config ./openssl.cnf -ss_cert pem/AdobeAUM_rootCert.pem

# Give the Adobe root certificate a private key so that we can create a chain
openssl genrsa -des3 -passout pass:secret1 -out tmp.key 1024
openssl rsa -in tmp.key -passin pass:secret1 -passout pass:secret1 -out AdobeAUM_rootCert.key
rm tmp.key

# Add AdobeUpdate.pem to our chain
#openssl req -config ./openssl.cnf -new -keyout AdobeUpdate.key -out AdobeUpdate.request -days 365 -passin pass:secret1 -passout pass:secret1 
#openssl ca -config ./openssl.cnf -cert pem/AdobeAUM_rootCert.pem -keyfile AdobeAUM_rootCert.key -out AdobeUpdate-signed.pem -infiles AdobeUpdate.request


################################
# Create a signer certificate  #
################################

# Create the request
openssl req -batch -config ./openssl.cnf -new -keyout signer-certificate.key -out signer-certificate.request -days 365 -passin pass:secret1 -passout pass:secret1 

# Create the certificate 
openssl ca -batch -config ./openssl.cnf -policy policy_anything -passin pass:secret1 -out signer-certificate.pem -infiles signer-certificate.request

# Cleanup request file
rm signer-certificate.request

####################
# Start the server #
####################

# Use signer-certificate.pem as response signer.
#openssl ocsp -CA CA/cacert.pem -index CA/index.txt -rkey signer-certificate.key -rsigner signer-certificate.pem -text -port 5000

# The client request:
# $ openssl ocsp -issuer CA/cacert.pem -VAfile signer-certificate.pem -cert thirdparty.pem -url http://127.0.0.1:5000
# => Response verify OK
# => thirdparty.pem: unknown
#        This Update: Apr 13 13:00:59 2010 GMT

