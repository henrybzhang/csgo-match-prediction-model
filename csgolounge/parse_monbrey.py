from lxml import html
import requests
import time
import os.path

url = 'http://monbrey.com.au/csgl/'

def read_table():
	f = open('monbrey_stats.txt', 'w')
	stats_page = requests.get(url)
	stats_page_tree = html.fromstring(stats_page.content)

	date = stats_page_tree.xpath('//tr[@role="row"]/td[2]/text()')
	winning_team = stats_page_tree.xpath
	team1 = stats_page_tree.xpath('//tr[@role="row"]/td[5]/text()')
	team2 = stats_page_tree.xpath('//tr[@role="row"]/td[6]/text()')
