from lxml import html
import requests
import time
import os.path

url = 'http://monbrey.com.au/csgl/'

def read_table():
	stats_page = requests.get(url)
	stats_page_tree = html.fromstring(stats_page.content)

	teamA_wins = stats_page_tree.xpath('//tr/td[5]/@class')
	teamB_wins = stats_page_tree.xpath('//tr/td[6]/@class')
	teamA = stats_page_tree.xpath('//tr/td[5]/text()')
	teamB = stats_page_tree.xpath('//tr/td[6]/text()')

	teamA = [x.split('(')[1] for x in teamA]
	teamA = [x[:-2] for x in teamA]
	teamB = [x.split('(')[1] for x in teamB]
	teamB = [x[:-2] for x in teamB]

	teamA_wins = [x.split()[0] for x in teamA_wins]
	for i in range(len(teamA_wins)):
		if teamA_wins[i] == 'teamA':
			teamA_wins[i] = '0'
		else:
			teamA_wins[i] = '1'

	f = open('monbrey_stats.txt', 'w')
	for A, B, percentA, percentB in zip(teamA_wins, teamB_wins, teamA, teamB):
		if B == 'teamB' and A == '0':
			continue
		print >>f, A, percentA, percentB
	f.close()

read_table()