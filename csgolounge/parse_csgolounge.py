from lxml import html
import requests
import time
import os.path

headers = {
	'User-Agent': 'Mozilla/5.0'
}

base_url = 'https://csgolounge.com/match?m='

def get_match_links():
	f = open('csgolounge_matches.txt', 'w')
	for x in range(11400, 12000):
		url = base_url + str(x)

		match_page = requests.get(url)
		match_page_tree = html.fromstring(match_page.content)

		teams = match_page_tree.xpath('//span[@class="match-team-name"]/text()')
		percentages = match_page_tree.xpath('//div[@class="match-team-percent"]/text()')

		winner1 = match_page_tree.xpath('//div[@class="match-trapezoid-winner-left"]/text()')
		winner2 = match_page_tree.xpath('//div[@class="match-trapezoid-winner-right"]/text()')

		if not teams:
			print "ERROR: Match Number " + str(x)
			break

		if percentages[0] == "50%" or percentages[0] == "0%" or percentages[0] == "100%":
			print "Incorrect Percentage"
			continue

		if not winner1:
			print >>f, 0,
		else:
			print >>f, 1,

		for t, p in zip(teams, percentages):
			print >>f, t, p,

		print >>f

		print teams, percentages

get_match_links()