# Introduction
#### Elo Rating System
The Elo rating system is a famous method invented by Arpad Elo to rate chess players. However, this rating system can apply to any skill-based game, i.e League of Legends, Go. A player's rating is determined by the results of every game they play. After playing each game, the score a player was expected to receive is compared to the actual score received. The larger the difference between them, the greater the change in the player’s rating. The general form of the equation is **New Rating = Old Rating + K * (Actual Score - Expected Score)**.
The constant **K** affects the influence each game has on rating.
#### Counter Strike: Global Offensive
Counter Strike: Global Offensive, or CSGO, is a famous first-person shooter game with almost 10 million unique players each month. In the game, there are two factions, Terrorists and Counter-Terrorists. The objective of the Terrorists is to either plant the bomb and have it explode or kill all the Counter-Terrorists.
In the competitive mode, there are two teams, each having five players. For fifteen rounds, one team are the Terrorists while the other team are the Counter-Terrorists. At halftime, or once fifteen rounds are played, the teams switch; the team that was Terrorists become Counter-Terrorists and vice versa. The victor is decided when a team wins sixteen rounds.
# Application
This project uses the Elo rating system to predict the outcome of CSGO pro player matches. The goal is to become a better predictor than crowd percentages on CSGOLounge, a match betting site.

The data used in this project is scraped from HLTV.org using the requests library from python. The database is at [HLTV Statistics Page](http://www.hltv.org/?pageid=188).

First, the data is formatted. Pro player in-game names are parsed from the original data file and placed into a separate text file. Matches are sorted by the date they were played.
	
Currently, the program only makes a prediction on a match when each player in it has played at least 30 matches beforehand. From this, there is a total of 584 different pro players and 3160 matche outcomes being predicted.
The basic implementation of the Elo rating system using **K**=0.3 predicts 2021 games correctly with a mean squared error 0.331145. On the other hand, suprisingly,  **K**=0.1 predicts 2013 games correctly with a mean squared error of 0.318689.

Additionally, there are two other programs that add on to the basic implementation: `with_date.cpp` weights a player’s match score history based on how far into the past it was played when determining player ratings; `with_map.cpp` gives each player a different rating on all (8) maps.

