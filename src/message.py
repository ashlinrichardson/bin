'''20170619 adapted from https://docs.python.org/2/library/smtplib.html '''
import smtplib
from fl0w3r import error, args
from ansicolor import KRED, KGRN

def message(fromaddr, toaddrs, msg):
    print KRED + "FROM: " + KGRN + fromaddr
    print KRED + " TO:" + KGRN + toaddrs 
    print KRED + " MSG: " + KGRN + str(msg).strip()
    ''' send an email with body "msg" '''
    msg = 'From: ' + fromaddr + '\r\nTo: ' + toaddrs + '\r\n\r\n' + msg
    server = smtplib.SMTP('smtp.telus.net')
    server.set_debuglevel(1)
    server.sendmail(fromaddr, toaddrs, msg)
    server.quit()

def prompt(prompt):
    return raw_input(prompt).strip()

if __name__ == '__main__':
    fromaddr, toaddrs, msg = None, None, None
    if len(args) > 3:
        fromaddr = args[1]
        toaddrs = args[2]
        msg = ' '.join(args[3:])
    else:
        fromaddr = prompt("From: ")
        toaddrs  = prompt("To: ") # .split()
        print "Enter message, end with ^D (Unix) or ^Z (Windows):"
        msg = ""
        while 1:
            try:
                line = raw_input()
                msg += line
            except EOFError:
                break
            if not line:
                break
    # send the message
    print "Message length is " + repr(len(msg))
    message(fromaddr, toaddrs, msg)
