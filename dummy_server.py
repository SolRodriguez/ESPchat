import sqlite3
import datetime 
import numpy as np 
db_name = "__HOME__/dummy_project/dummy_server.db"

def create_db():
	conn = sqlite3.connect(db_name)
	c = conn.cursor()
	c.execute('''CREATE TABLE IF NOT EXISTS audios_table (user text, encoding text, timing timestamp);''')
	conn.commit()
	conn.close()

def request_handler(request):
	if request["method"] == "POST":
		conn = sqlite3.connect(db_name)
		create_db()
		new_request = "".join(chr(x) for x in request["data"])
		username = ""
		sd = ""
		index_o_i = 0
		for index, letter in enumerate(new_request[5:]):
			if letter != "&":
				username += letter
			else:
				index_o_i = index + 12
				sd = new_request[5:][index+4]
				break
		if sd == "1":  	
			encoding = new_request[5:][index_o_i:]
			stamp = datetime.datetime.now()
			c = conn.cursor()
			c.execute('''INSERT INTO audios_table VALUES(?,?,?);''', (username,encoding,stamp))
			conn.commit()
			conn.close()
			return "WHOOO"
		elif sd == "0":
			c = conn.cursor()
			send_back = c.execute('''SELECT encoding FROM audios_table WHERE user=? ORDER BY timing DESC LIMIT 1;''', (username,)).fetchone()
			conn.commit()
			conn.close()
			if send_back != None:
				return send_back[0]
			else:
				return "User does not exist"
