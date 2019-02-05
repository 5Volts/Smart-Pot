from django.http import HttpResponse
from notify_run import Notify

notify = Notify()

def index(request,value):
	notify.send(f'Your plant is getting dry. Analog Value of {value} detected on sensor.')
	return HttpResponse('<p>Send</p>')