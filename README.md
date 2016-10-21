# Introduction
### Elo Rating System
The Elo rating system is a famous method invented by Arpad Elo to rate chess players. However, this rating system can apply to any skill-based game, i.e League of Legends, Go. The system rates players by the results of every game they play. After playing each game, the score a player was expected to receive is compared to the actual score he received. The larger the difference between them, the larger the player’s rating will change. The general form of the equation:
**New Rating = Old Rating + K * (Actual Score - Expected Score)**
**K** affects how influential each game in changing the rating.
### Counter Strike: Global Offensive
Counter Strike: Global Offensive, or CSGO, is a famous first-person shooter game with almost 10 million unique players each month. In the game, there are two factions, Terrorists and Counter-Terrorists. The objective of the Terrorists is to either plant the bomb and have it explode or kill all the Counter-Terrorists.
In the competitive mode, there are two teams, each having five players. For fifteen rounds, one team are the Terrorists while the other are the Counter-Terrorists. At halftime, or once fifteen rounds are played, the teams switch; the team that was Terrorists become Counter-Terrorists and vice versa. The victor is decided when a team reaches sixteen round wins.
# Application
This project uses the Elo rating system to predict the outcomes of CSGO pro player matches. The goal is to be a better predictor than the percentages on CSGOLounge, a match betting site.

The data used in this project is scraped from HLTV.org using the requests library from python. The database is at [HLTV Statiscs Page](http://www.hltv.org/?pageid=188).

The data is first formatted to make the process easier. Pro player in-game names are parsed from the original data file and placed into a separate text file. The match data is sorted by date played.
	
Currently, the program makes sure each player in a match has played at least 30 matches beforehand before attempting to make a prediction. From this, there is a total of 584 different pro players and 3160 matches being predicted.
The basic implementation of the Elo rating system using **K**=0.3 predicts 2021 games correctly with a mean squared error 0.331145. On the other hand, **K**=0.1 predicts 2013 games correctly with a mean squared error of 0.318689.

Additionally, there are two other programs that add on to the basic implementation: ‘’’with_date.cpp’’’ weights a player’s match score history based on how far into the past it was played when determining player ratings; ‘’’with_map.cpp’’’ gives each player a different rating on all the maps (total of 8).

