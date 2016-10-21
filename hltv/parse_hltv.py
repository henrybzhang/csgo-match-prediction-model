from lxml import html
import requests
import time
import os.path

headers = {
	'User-Agent': 'Mozilla/5.0'
}

match_links = []
month = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]

def get_match_links():
	start_page = requests.get('http://www.hltv.org/?pageid=188', headers=headers)
	start_page_tree = html.fromstring(start_page.content)
	N_matches = start_page_tree.xpath('string(//div[@class="tab_groups"]/div[last()]/div/div/div/text()[3])')
	N_matches = int(filter(str.isdigit, N_matches))
	#N_matches = 20277
	N_pages = int(N_matches / 50 + 1)

	global match_links
	for i in range(N_pages):
		time.sleep(1.5)
		stats_page = requests.get('http://www.hltv.org/?pageid=188' + '&offset=' + str(i * 50), headers=headers)
		stats_page_tree = html.fromstring(stats_page.content)

		match_links += stats_page_tree.xpath('//div[@style="padding-left:5px;padding-top:5px;"]/a[1]/@href')
		print 'http://www.hltv.org/?pageid=188' + '&offset=' + str(i * 50)
	
	f = open('match_links.txt', 'w')
	for i in range(len(match_links)):
		print >>f, match_links[i]
	f.close()	

def input_match_links():
	f = open('match_links.txt', 'r')
	match_links = f.readlines()
	f.close()
	match_links = [x.replace('\n', '') for x in match_links]

def get_match_data():
	f = open('data.txt', 'w')
	#match_links = ['/?pageid=188&matchid=15000']

	for i in range(len(match_links)):
		time.sleep(1.5)
		match_page = requests.get('http://www.hltv.org' + match_links[i], headers=headers)
		match_page_tree = html.fromstring(match_page.content)

		map_name = match_page_tree.xpath('string(//div[@style="font-weight:normal;width:180px;float:left;text-align:right;"]/text())')
		map_score = match_page_tree.xpath('//div[@style="font-weight:normal;width:180px;float:left;text-align:right;"]/span[position()<3]/text()')
		date = match_page_tree.xpath('string(//div[@style="font-weight:normal;width:230px;float:left;text-align:right;"]/text())')
		team_names = match_page_tree.xpath('//img[@class="flagFix"]/../text()')
		players = match_page_tree.xpath('//div[@style="padding-left:5px;padding-top:5px;"]/div[1]/a/text()')
		team = match_page_tree.xpath('//div[@style="font-weight:normal;width:20%;float:left;text-align:center"]/a/text()')
		kills = match_page_tree.xpath('//span[@title="headshots"]/../text()')
		headshots = match_page_tree.xpath('//span[@title="headshots"]/text()')
		assists = match_page_tree.xpath('//div[@style="padding-left:5px;padding-top:5px;"]/div[4]/text()')
		deaths = match_page_tree.xpath('//div[@style="padding-left:5px;padding-top:5px;"]/div[5]/text()')
		hltv_rating = match_page_tree.xpath('//div[@style="padding-left:5px;padding-top:5px;"]/div[9]/text()')

		#format data
		team_names = [x.replace(' ', '', 1) for x in team_names]
		team_names = [x.replace(' ', '_') for x in team_names]
		team = [x.replace(' ', '_') for x in team]
		players = [x.replace(' ', '') for x in players]
		headshots = [x.replace('(', '') for x in headshots]
		headshots = [x.replace(')', '') for x in headshots]
		hour_of_day = date[11:13];
		print date[5:7] + '-' + date[8:10] + '-' + date[:4], hour_of_day
		date = (int(date[:4]) - 2012) * 365 + (int(date[5:7])) * month[int(date[5:7]) - 1] + (int(date[8:10]) - 1)
		
		if len(map_score) != 2:
			print "Error: map_score"
			print 'http://www.hltv.org' + match_links[i], '\n'
		if len(team_names) != 2:
			print "Error: team_names"
			print 'http://www.hltv.org' + match_links[i], '\n'

		if len(players) != 10:
			print "Error: players"
			print 'http://www.hltv.org' + match_links[i], '\n'
			continue
		if len(team) != 10:
			print "Error: team"
			print 'http://www.hltv.org' + match_links[i], '\n'
			continue
		if len(kills) != 10:
			print "Error: kills"
			print 'http://www.hltv.org' + match_links[i], '\n'
			continue
		if len(headshots) != 10:
			print "Error: headshots"
			print 'http://www.hltv.org' + match_links[i], '\n'
			continue
		if len(assists) != 10:
			print "Error: assists"
			print 'http://www.hltv.org' + match_links[i], '\n'
			continue
		if len(deaths) != 10:
			print "Error: deaths"
			print 'http://www.hltv.org' + match_links[i], '\n'
			continue
		if len(hltv_rating) != 10:
			print "Error: hltv_rating"
			print 'http://www.hltv.org' + match_links[i], '\n'
			continue
		
		#print players, team, kills, headshots, assists, deaths, hltv_rating

		print >>f, date, hour_of_day, map_name,
		for n, s in zip(team_names, map_score):
			print >>f, n, s,
		for p, t, k, h, a, d, h_r in zip(players, team, kills, headshots, assists, deaths, hltv_rating):
			print >>f, p, t, k, h, a, d, h_r,

		print >>f, '\n', '\n'

	f.close()


if os.path.exists('match_links.txt'):
	input_match_links()
else:
	get_match_links()
	
get_match_data()