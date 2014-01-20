<%@ page language=javascript %><%!
/**
 * Simple implementation of tic-tac-toe illustrating
 * dividing implementation into files.
 *
 * The session variable stores:
 *   user_score  -- the user's score
 *   comp_score  -- the computer's score
 *   state       -- the state of the game stored as an array.
 *               -- 0 = no move, 'x' user, 'o' computer
 *
 * User input comes through the query string:
 *  ?move=n
 */

/*
 * Exception handling is done by error.jsp.
 *
 * All calculations are done before creating any of the output so the
 * error handling doesn't mix up generated text and error text.
 */

import score; // import the functions in score.es

var state = session.value.state;
/*
 * If the player hasn't played before, initialize the state.
 */
if (! state) {
  session.value.user_score = 0;
  session.value.comp_score = 0;
  session.value.state = [0, 0, 0, 0, 0, 0, 0, 0, 0];
  state = session.value.state;
}
/*
 * If no moves are available, reinitialize the state.
 */
for (i = 0; i < state.length; i++) {
  if (state[i] == 0)
    break;
}
if (i == state.length) {
  state = session.value.state = [0, 0, 0, 0, 0, 0, 0, 0, 0];
}

var move = request.form.move;
/*
 * Response stores the message to tell the user based on the state
 * change, e.g. win, lose, bad move.
 */
var response = null;

/*
 * Interpret the user's move and calculate the computer's move.
 *
 * The AI behind the computer's move is in score.es.
 */
if (! move) {
}
else if (move >= 0 && move <= 8) {
  if (state[move] == 0) {
    state[move] = 'x';

    if (isUserWin(score(state))) {
      response = "user_win";
      session.value.user_score++;
      session.value.state = [0, 0, 0, 0, 0, 0, 0, 0, 0];
    }
    else if (isComputerWin(final = computer_move(state))) {
      response = "computer_win";
      session.value.comp_score++;
      session.value.state = [0, 0, 0, 0, 0, 0, 0, 0, 0];
    } else if (isTie(final)) {
      response = "tie";
      session.value.state = [0, 0, 0, 0, 0, 0, 0, 0, 0];
    }
  }
  else {
    response = "bad_move";
  }
} else {
  writeln("move: " + move);
  response = "bad_move";
}

/**
 * HTML for a single square.
 *
 * Legal moves are actually links with the proper move, e.g. the center
 * square would be:
 *
 *   <td><a href='tictactoe.jsp?move=5'>&nbsp;&nbsp;</a>
 */
function square(index)
{
  var value = state[index];

  if (value == 'x') {
    out.write("<td style='color:blue'>X</td>");
  }
  else if (value == 'o') {
    out.write("<td style='color:red'>O</td>");
  }
  else {
    out.write('<td><a href="tictactoe.jsp?move=', index, '">&nbsp;&nbsp;</a></td>');
  }
}

/**
 * HTML for the entire board.
 */
function board()
{
  out.writeln("<table border cellpadding=3>");
  out.write("<tr>"); square(0); square(1); square(2); out.writeln();
  out.write("<tr>"); square(3); square(4); square(5); out.writeln();
  out.write("<tr>"); square(6); square(7); square(8); out.writeln();
  out.writeln("</table>");
}

/*
 * Now that all calculations are done, we can safely create the page.
 */
%>
<html><head><title>TicTacToe</title></head>
<body bgcolor=white>

<!-- Easy way to get back without unwinding the state. -->
<a href='../index.html'>[back]</a>

<!-- Tell the player how well she's doing. -->

<table>
<tr><th>You:<td><%= session.value.user_score %>
<tr><th>Computer:<td> <%= session.value.comp_score %>
</table>

<%

/*
 * Prompt based on the state of the board.
 */
switch (response) {
case "user_win":
  %><h1>You Win!</h1><%
  board();
  %><h1>New Game:</h1><%
  break;

case "computer_win":
  %><h1>You Lose!</h1><%
  board();
  %><h1>New Game:</h1><%
  break;

case "tie":
  %><h1>Tie game!</h1><%
  board();
  %><h1>New Game:</h1><%
  break;

case "bad_move":
  %><h1>Bad Move.</h1><%
  break;

default:
  %><h1>Your Move.</h1><%
  break;
}

state = session.value.state;

/*
 * draw the board
 */
board();
%>

</body></html>
