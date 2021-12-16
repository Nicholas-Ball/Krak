from time import time
import urllib.parse
import hashlib
import hmac
import argparse
import os
import base64


def sign(urlpath, data,nonce, secret):
    encoded = (nonce + data).encode()
    message = urlpath.encode() + hashlib.sha256(encoded).digest()

    mac = hmac.new(base64.b64decode(secret.encode()), message, hashlib.sha512)
    sigdigest = base64.b64encode(mac.digest())
    return sigdigest.decode()

def main():
	#Get data from flags
	parser = argparse.ArgumentParser(description='Make a signture')
	parser.add_argument('-S', dest='sec',type=str,help='set secret as base 64')
	parser.add_argument('-N', dest='nonce',type=str,help='set nounce')
	parser.add_argument('-C', dest='command',type=str,help='set command')
	parser.add_argument('-P', dest='path',type=str,help='set uri path')
	args = parser.parse_args()

	if(args.command == "null"):
		args.command = "nonce="+args.nonce

	sig = sign(args.path,args.command,args.nonce,args.sec)

	#open file
	file = open("sign.txt","w")
	#write signiture to file
	file.write(str(sig))
	#close file
	file.close()

main()