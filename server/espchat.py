import sqlite3
import requests

user_hist = "__HOME__/espchat/posts.db"


def create_db():
    '''Creates the database of '''
    conn = sqlite3.connect(user_hist)
    c = conn.cursor()

    c.execute(
        '''CREATE TABLE IF NOT EXISTS user_hist (user text, pic text, audio text);''')  # will add new entries for vid/audio once we have the hardware
    conn.commit()

    return c, conn


def user_exists(user, db):
    return db.execute('''SELECT EXISTS(SELECT 1 FROM user_hist WHERE user=?);''', (user,)).fetchone()[0]


def display_resp(responses):
    '''Formats the list of tuples into usable strings, etc.
    '''
    return '\n'.join([x[0] for x in responses])


def request_handler(request):
    # creates database and keeps conn open
    userdb, conn = create_db()

    if request['method'] == 'POST':

        user = request['form']['user']

        # 8bit int arrays
        img = request['form']['img']
        audio = request['form']['audio']

        # check if the user has posted before
        if user_exists(user, userdb):  # update
            upload = userdb.execute(
                '''UPDATE user_hist SET pic = ?, audio = ? WHERE user = ?;''', (img, audio, user))
            conn.commit()

        else:  # if the user deosn't exists we insert
            upload = userdb.execute(
                '''INSERT INTO user_hist VALUES(?,?,?);''', (user, img, audio))
            conn.commit()

        conn.close()
        return audio

    elif request['method'] == 'GET':
        # indicator that we're looking for usernames to display on the menu
        if request['values']['menu'] == 'True':
            users = userdb.execute(
                '''SELECT DISTINCT(user) FROM user_hist;''',).fetchall()
            return display_resp(users)

        else:
            selection = request['values']['selection']
            download = userdb.execute(
                '''SELECT pic, audio FROM user_hist WHERE user = ?;''', (selection,)).fetchall()[0]
            conn.close()
            return download
