#-*- coding:utf-8 -*-
import time
import math

monthdays = [0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]

# ===========================================================
#     Determine if a year is a leap year
# ===========================================================
def leapyear(year):
    if year % 4 == 0 and year % 100 != 0:
        return 1
    if year % 400 == 0:
        return 1
    return 0

# ===========================================================
#     Change year,day of year to year,month,day
# ===========================================================
def doy2ymd(year, doy):
    day = doy
    mon = 0
    for i in range(13):
        monthday = monthdays[i]
        if (i == 2 and leapyear(year) == 1):
            monthday += 1
        if day > monthday:
            day -= monthday
        else:
            mon = i
            break

    return year,mon,day

# ===========================================================
#     Change year,month,day to year,day of year
# ===========================================================
def ymd2doy(year, mon, day):
    doy = day
    for i in range(1, mon):
        doy += monthdays[i]
    if (mon > 2):
        doy += leapyear(year)
    return year,doy

# ===========================================================
#     Change year,month,day to Modified Julian Day
# ===========================================================
def ymd2mjd(year, mon, day):
    mjd = 0.0
    if (mon <= 2):
        mon += 12
        year -= 1
    mjd = 365.25 * year - 365.25 * year % 1.0 - 679006.0
    mjd += math.floor(30.6001 * (mon + 1)) + 2.0 - math.floor(
        year / 100.0) + math.floor(year / 400) + day
    return mjd

