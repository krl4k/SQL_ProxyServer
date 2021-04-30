import psycopg2
conn = psycopg2.connect(dbname='postgres', user='postgres',
                        password='postgres', host='localhost')
cursor = conn.cursor()

cursor.execute('SELECT * FROM person')
records = cursor.fetchall()

print(records)

cursor.close()
conn.close()