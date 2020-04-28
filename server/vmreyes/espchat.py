import sqlite3
import datetime
import requests
import numpy
from html import unescape

USER_DATA = "__HOME__/espchat_db/user_data.db"

def request_handler(request):
    user_data_db, conn = begin_db()

    # get requests ask for sync info of the user
    if request["method"] == "GET":
        pass
        #return handle_get(request, shard_db, conn)
    else:
        return handle_post(request, user_data_db, conn)


def handle_get(request, db, conn):
    pass

def handle_post(request, db, conn):
    data = request['data']
    #decode and convert to string to break up
    # we can't decode :(
    username = data[5:data.find(b'&img=')].decode('utf-8')
    img_data = data[data.find(b'&img=')+5:data.find(b'&audio=')]
    audio_data = data[data.find(b'&audio=')+7:]

    if user_exists(username, conn):
        conn.execute('''UPDATE user_data SET video=?, audio=? WHERE user=?;''', (img_data, audio_data, username))
    else:
        conn.execute('''INSERT INTO user_data VALUES(?,?,?);''', (username, img_data, audio_data))

    conn.commit()
    conn.close()
    return "SUCCESS"


def begin_db():
    conn = sqlite3.connect(USER_DATA)
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS user_data (user text, video text, audio text);''')
    c.commit()
    return c, conn

def user_exists(user, db):
    return db.execute('''SELECT EXISTS(SELECT 1 FROM user_data WHERE user=?);''',(user,)).fetchone()[0]
