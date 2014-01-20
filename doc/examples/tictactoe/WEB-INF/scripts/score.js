/*
 * score.es
 *
 * Artificial intelligence for tic-tac-toe.
 *
 * The AI is simple.  The computer tries all its legal moves followed
 * by a user move and selects the computer move with the the best score.
 */

// wins lists all the winning triplets.

var wins = [[0, 1, 2], [3, 4, 5], [6, 7, 8],
            [0, 3, 6], [1, 4, 7], [2, 5, 8],
            [0, 4, 8], [2, 4, 6]];

// AI weights for number of marks by a single player in a row.
// 3 in a row is a win.

var scores = [0, 1, 1, 100];

/**
 * Calculate the score of a game state.
 *
 * Scores are from the computer's perspective, higher is better.
 *
 * The heuristic is extremely simple-minded.  Any row with only a
 * single player's marks is given a score based on the number of marks.
 * These are added up.
 */
function score(state)
{
  var score = 0;

 loop:
  for (var i = 0; i < wins.length; i++) {
    var win = wins[i];

    var user_count = 0;
    var comp_count = 0;
    for (var j = 0; j < win.length; j++) {
      switch (state[win[j]]) {
      case 'x': 
	user_count++;
	break;

      case 'o': 
	comp_count++;
	break;
      }
    }

    if (user_count == 0)
      score += scores[comp_count];
    else if (comp_count == 0) 
      score -= scores[user_count];
  }

  return score;
}

/**
 * Returns true if the score represents a user win.
 */
function isUserWin(score) { return score < -50; }

/**
 * Returns true if the score represents a computer win.
 */
function isComputerWin(score) { return score > 50; }

/**
 * Returns true if the score represents a tie.
 */
function isTie(score) { return score == -1000; }

/**
 * Returns the cost of best user move, i.e. the worst from 
 * the computer's perspective.
 */
function user_move(state)
{
  var best_move = null;
  var best_cost = 1000;

  for (var i = 0; i < state.length; i++) {
    if (state[i] == 0) {
      state[i] = 'x';
      var cost = score(state);
      state[i] = 0;
      if (cost < best_cost) {
	best_cost = cost;
	best_move = i;
      }
    }
  }

  return best_cost;
}

/**
 * Calculates the best computer move, returning the end score.
 *
 * The state is updated with an 'o' where the computer moves.
 */
function computer_move(state)
{
  var best_move = null;
  var best_cost = -1000;

  for (var i = 0; i < state.length; i++) {
    if (state[i] == 0) {
      state[i] = 'o';
      var comp_cost = score(state);

      if (comp_cost > 50)
        var cost = comp_cost;
      else
        var cost = user_move(state);

      state[i] = 0;
      if (cost > best_cost) {
	best_cost = cost;
	best_move = i;
      }
    }
  }

  if (best_move == null)
    return -1000;

  state[best_move] = 'o';

  return score(state);
}
