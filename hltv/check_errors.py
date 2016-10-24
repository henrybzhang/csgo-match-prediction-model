from lxml import html
import requests
import time
import os.path
import sys
reload(sys)
sys.setdefaultencoding('utf-8')

headers = {
	'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; rv:32.0) Gecko/20100101 Firefox/32.0'
}

match_links = []
month = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]

link = False

def line_by_line():
	global link
	with open('error.txt', 'r') as f:
		for line in f:
			line = line.rstrip()
			if(line == ''):
				continue;
			if(link == True):
				get_match_data(line)
			if(line[:5] == 'Error'):
				link = True;
				continue

def get_match_data(line):
	global link
	time.sleep(2)
	match_page = requests.get(line, headers=headers)
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
	date = (int(date[:4]) - 2012) * 365 + (int(date[5:7])) * month[int(date[5:7]) - 1] + (int(date[8:10]) - 1)

	if len(map_score) != 2:
		print "Error: map_score", len(map_score)
		print line
		link = False
		return
	if len(team_names) != 2:
		print "Error: team_names", len(team_names)
		print line
		link = False
		return

	if len(players) != 10:
		print "Error: players", len(players)
		print line
		link = False
		return
	if len(team) != 10:
		print "Error: team", len(team)
		print line
		link = False
		return
	if len(kills) != 10:
		print "Error: kills", len(kills)
		print line
		link = False
		return
	if len(headshots) != 10:
		print "Error: headshots", len(headshots)
		print line
		link = False
		return
	if len(assists) != 10:
		print "Error: assists", len(assists)
		print line
		link = False
		return
	if len(deaths) != 10:
		print "Error: deaths", len(deaths)
		print line
		link = False
		return
	if len(hltv_rating) != 10:
		print "Error: hltv_rating", len(hltv_rating)
		print line
		link = False
		return
	
	print line, 'Fixed'
	link = False

line_by_line()