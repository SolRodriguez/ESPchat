import sqlite3
import requests

user_hist = "__HOME__/espchat/posts.db"


def create_db():
    '''Creates the database of '''
    conn = sqlite3.connect(user_hist)
    c = conn.cursor()

    c.execute(
        '''CREATE TABLE IF NOT EXISTS user_hist (user text, pic VARBINARY(MAX), audio VARBINARY(MAX));''')  # will add new entries for vid/audio once we have the hardware
    conn.commit()

    return c, conn


def user_exists(user, db):
    return db.execute('''SELECT EXISTS(SELECT 1 FROM user_hist WHERE user=?);''', (user,)).fetchone()[0]


def display_resp(responses):
    '''Formats the list of tuples into usable strings, etc.
    '''
    return '\n'.join([x[0] for x in responses])


def handle_post(request, db, conn):
    data = request['data']
    # decode and convert to string to break up
    # we can't decode :(
    username = data[5:data.find(b'&img=')].decode('utf-8')
    img_data = data[data.find(b'&img=')+5:data.find(b'&audio=')]
    audio_data = data[data.find(b'&audio=')+7:]

    # check if the user has posted before
    if user_exists(username, db):  # update
        upload = db.execute(
            '''UPDATE user_hist SET pic = ?, audio = ? WHERE user = ?;''', (img_data, audio_data, username))
        conn.commit()

    else:  # if the user deosn't exists we insert
        upload = db.execute(
            '''INSERT INTO user_hist VALUES(?,?,?);''', (username, img_data, audio_data))
    
    conn.commit()
    conn.close()
    return "SUCCESS"

def request_handler(request):
    # creates database and keeps conn open
    userdb, conn = create_db()

    if request['method'] == 'POST':
        return handle_post(request,userdb,conn)

    elif request['method'] == 'GET':
        # indicator that we're looking for usernames to display on the menu
        if request['values']['menu'] == 'True':
            users = userdb.execute(
                '''SELECT DISTINCT(user) FROM user_hist;''',).fetchall()
            return display_resp(users)

        else:
            user = request['values']['user']
            download = userdb.execute(
                '''SELECT pic, audio FROM user_hist WHERE user = ?;''', (user,)).fetchall()[0]
            conn.close()
            return download
