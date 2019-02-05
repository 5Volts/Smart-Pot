from . import views
from django.urls import path

urlpatterns = [
    path('<int:value>',views.index,name='index'),
]