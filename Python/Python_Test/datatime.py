import datetime
import numpy as np

tNow = datetime.datetime.strptime("20180108", "%Y%m%d")
print(tNow)
print(f"tNow = {tNow.strftime('%Y%m%d')}")

nPre = tNow - datetime.timedelta(1)
print(nPre)
print(f"nPre = {nPre.strftime('%Y%m%d')}")

print(tNow.strftime("%Y%m%d"))

the_date = '20190108'
the_date_30daybefore = (datetime.datetime.strptime(the_date, '%Y%m%d') - datetime.timedelta(30)).strftime('%Y%m%d')
print(the_date_30daybefore)