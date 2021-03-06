import os
import datetime

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd, prepareUsersCurrentHoldingsForDisplay

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""

    thisUser = session["username"]

    usersCurrentHoldings = {}

    usersCurrentHoldings["holdings"] = db.execute(
        "SELECT Ticker, Shares FROM Holdings WHERE User = :user", user=thisUser)

    currentCashBalance = db.execute("SELECT cash FROM users WHERE username = :username",
                                    username=thisUser)

    currentCashBalance = currentCashBalance[0]["cash"]

    usersPortfolioValue = 0

    if usersCurrentHoldings["holdings"]:

        # Transform list of dictionaries into a dictionary of dictionaries
        usersCurrentHoldings["holdings"] = {dictEntry["Ticker"]: {
            "Shares": dictEntry["Shares"]} for dictEntry in usersCurrentHoldings["holdings"]}

        usersCurrentHoldings = prepareUsersCurrentHoldingsForDisplay(
            usersCurrentHoldings)

        if "error" in usersCurrentHoldings:
            return apology(lookupResults["error"], 403)

        totalValueOfUsersStocks = 0

        for holding in usersCurrentHoldings["holdings"]:
            # Remove $ from price string returned in lookupResults
            thisPrice = float(
                usersCurrentHoldings["holdings"][holding]["Price"][1:])
            thisValue = usersCurrentHoldings["holdings"][holding]["Shares"] * thisPrice
            totalValueOfUsersStocks = totalValueOfUsersStocks + thisValue

            thisValue = usd(thisValue)
            usersCurrentHoldings["holdings"][holding]["Value"] = thisValue

        usersPortfolioValue = currentCashBalance + totalValueOfUsersStocks

    return render_template("index.html", usersCurrentHoldings=usersCurrentHoldings, currentCashBalance=usd(currentCashBalance), usersPortfolioValue=usd(usersPortfolioValue), thisUser=thisUser)


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""

    thisUser = session["username"]

    usersCurrentHoldings = {}

    usersCurrentHoldings["holdings"] = db.execute(
        "SELECT Ticker, Shares FROM Holdings WHERE User = :user", user=thisUser)

    # Transform list of dictionaries into a dictionary of dictionaries
    usersCurrentHoldings["holdings"] = {dictEntry["Ticker"]: {
        "Shares": dictEntry["Shares"]} for dictEntry in usersCurrentHoldings["holdings"]}

    if request.method == "GET":

        usersCurrentHoldings = prepareUsersCurrentHoldingsForDisplay(
            usersCurrentHoldings)

        return render_template("displayHoldings.html", usersCurrentHoldings=usersCurrentHoldings, parentPage="buy")

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        if not request.form.get("symbol"):
            return apology("must provide symbol", 403)

        elif not request.form.get("number"):
            return apology("must provide number", 403)

        usersTickerSymbol = request.form.get("symbol").upper()

        if len(usersTickerSymbol) > 5:
            return apology("Ticker symbol is too long.", 403)

        numberOfSharesUserOwns = 0

        if usersTickerSymbol in usersCurrentHoldings["holdings"]:
            numberOfSharesUserOwns = usersCurrentHoldings["holdings"][usersTickerSymbol]["Shares"]

        numberOfSharesToBuy = int(request.form.get("number"))

        # Return an error if the user enters zero or fewer shares to buy
        if numberOfSharesToBuy <= 0:
            return apology("Must purchase at least 1 share.", 403)

        thisUsersCash = db.execute("SELECT cash FROM users WHERE username = :username",
                                   username=thisUser)

        thisUsersCash = thisUsersCash[0]["cash"]

        apiSearchResults = lookup(usersTickerSymbol)

        if apiSearchResults == None:
            return apology("ticker symbol api error", 403)

        stockPrice = round(apiSearchResults["price"], 2)

        thisTransactionsTotal = stockPrice * numberOfSharesToBuy

        if thisTransactionsTotal > thisUsersCash:
            return apology("Not enough cash to make purchase.", 403)
        else:
            newCashBalance = thisUsersCash - thisTransactionsTotal

            db.execute("UPDATE users SET cash = :newCashBalance WHERE username = :username",
                       username=thisUser, newCashBalance=newCashBalance)

            # Update the number of shares the user now owns
            newNumberOfShares = numberOfSharesUserOwns + numberOfSharesToBuy

            if numberOfSharesUserOwns > 0:
                db.execute("UPDATE Holdings SET Shares=:shares WHERE User=:user AND Ticker=:ticker",
                           shares=newNumberOfShares, user=thisUser, ticker=usersTickerSymbol)
            else:
                db.execute("INSERT INTO Holdings (User, Ticker, Shares) VALUES (:username, :usersTickerSymbol, :numberOfSharesToBuy)",
                           username=thisUser, usersTickerSymbol=usersTickerSymbol, numberOfSharesToBuy=numberOfSharesToBuy)

            db.execute("INSERT INTO History (Ticker, Price, DateTime, Type, User, NumberOfShares) VALUES (:ticker, :price, :dateTime, :type, :user, :numberOfShares)",
                       ticker=usersTickerSymbol, price=stockPrice, dateTime=datetime.datetime.now().strftime("%d-%m-%Y %H:%M"), type="BUY", user=thisUser, numberOfShares=numberOfSharesToBuy)

        return render_template("messageDisplay.html", message=f"Purchased {numberOfSharesToBuy} shares of {usersTickerSymbol} at {usd(stockPrice)} per share for a total of {usd(thisTransactionsTotal)}.")


@app.route("/check", methods=["GET"])
def check():
    """Return true if username available, else false, in JSON format"""

    if request.method == "GET":

        if request.args.get("username"):

            testUsername = request.args.get("username")

            usernameInDatabase = db.execute(
                "SELECT * FROM users WHERE username = :testUsername", testUsername=testUsername)

            usernameTaken = False

            if usernameInDatabase:
                usernameTaken = True

            return jsonify(usernameTaken=usernameTaken)


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""

    thisUser = session["username"]

    usersHistory = db.execute("SELECT * FROM History WHERE User = :username",
                              username=thisUser)

    for record in usersHistory:

        record["DateTime"] = datetime.datetime.strptime(
            record["DateTime"], "%d-%m-%Y %H:%M")

        record["DateTime"] = datetime.datetime.strftime(
            record["DateTime"], "%x %I:%M")

    return render_template("history.html", usersHistory=usersHistory, thisUser=thisUser)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username",
                          username=request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]
        session["username"] = rows[0]["username"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""

    user = session["username"]

    # make a copy of all the existing records for the user from the database
    usersCurrentTickers = {}

    usersCurrentTickers["holdings"] = db.execute(
        "SELECT Ticker, QuoteNumber FROM Quotes WHERE User = :user", user=user)

    # Transform list of dictionaries into a dictionary of dictionaries
    usersCurrentTickers["holdings"] = {dictEntry["Ticker"]: {
        "QuoteNumber": dictEntry["QuoteNumber"]} for dictEntry in usersCurrentTickers["holdings"]}

    if request.method == "GET":
        # If usersCurrentTickers is empty, don't return a usersQuotes
        if usersCurrentTickers["holdings"]:
            usersCurrentTickers = prepareUsersCurrentHoldingsForDisplay(
                usersCurrentTickers)

        return render_template("printQuotes.html", usersQuotes=usersCurrentTickers)

    elif request.method == "POST":
        # if the usersListLength is empty and the user inputs nothing, return a custom error message

        # when list is full, when user inputs duplicate symbol and then submits a blank quote, the last quote is repeated

        # Prepare error message to user if incomplete form
        if request.form.get("symbol"):
            usersTickerSymbol = request.form.get("symbol").upper()
        else:
            usersCurrentTickers["error"] = "Please fill out ticker symbol."

        if not "error" in usersCurrentTickers:
            # Create error message if the length is too long
            if len(usersTickerSymbol) > 5:
                usersCurrentTickers["error"] = "Ticker symbol is too long."

            # Test whether usersTickerSymbol is already being tracked by the user
            if usersCurrentTickers["holdings"] and not "error" in usersCurrentTickers:
                if usersTickerSymbol in usersCurrentTickers["holdings"]:
                    # Create error because the ticker is already being tracked
                    usersCurrentTickers["error"] = f"{usersTickerSymbol} is already being tracked."

            # usersTickerSymbol is not already in list; it's safe to add it to usersCurrentTickers
            if not "error" in usersCurrentTickers:

                # Assign a new QuoteNumber for usersTickerSymbol
                thisNewQuoteNumber = len(usersCurrentTickers["holdings"]) + 1

                print("here111:", usersCurrentTickers)

                # Add usersTickerSymbol to holdings
                usersCurrentTickers["holdings"][usersTickerSymbol] = {
                    "QuoteNumber": thisNewQuoteNumber}

        # Add current per share price information from API
        usersCurrentTickers = prepareUsersCurrentHoldingsForDisplay(
            usersCurrentTickers)

        print("here1: ", usersCurrentTickers)

        # Only check for pricing presence of usersTickerSymbol if there was not a previous error (i.e. no symbol was entered)
        if not "error" in usersCurrentTickers:

            # Return an error message if the user did enter a ticker symbol, but it was invalid (not present in API results)
            if not "Price" in usersCurrentTickers["holdings"][usersTickerSymbol]:

                usersCurrentTickers["error"] = f"Unable to find ticker symbol {usersTickerSymbol}."
                # Delete usersTickerSymbol from usersCurrentTickers
                usersCurrentTickers["holdings"].pop(usersTickerSymbol)

            # Else, usersTickerSymbol is valid and database needs to be updated with the new ticker symbol
            else:
                listLengthIsSix = False

                # If there are more than five stocks being tracked, move each QuoteNumber down one (6 becomes 5, 1 becomes 0)
                if len(usersCurrentTickers["holdings"]) > 5:

                    listLengthIsSix = True

                    usersCurrentTickers["holdings"] = {
                        ticker: {"QuoteNumber": usersCurrentTickers["holdings"][ticker]
                                 ["QuoteNumber"] - 1, "Price": usersCurrentTickers["holdings"][ticker]["Price"]}
                        for ticker in usersCurrentTickers["holdings"] if usersCurrentTickers["holdings"][ticker]["QuoteNumber"]-1 > 0}

                    # Delete all of the user's existing database records so revised entries can be added
                    db.execute(
                        "DELETE FROM Quotes WHERE User = :user", user=user)

                # Test each entry to see if it needs to be added to the database
                for entry in usersCurrentTickers["holdings"]:

                    # If the length was six, each ticker needs to be added with its new QuoteNumber
                    # The usersTickerSymbol will be added to the database regardless of the size of usersCurrentTickers (it has passed API test)
                    if listLengthIsSix or entry == usersTickerSymbol:
                        db.execute("INSERT INTO Quotes (QuoteNumber, User, Ticker) VALUES (:quoteNumber, :user, :ticker)",
                                   quoteNumber=usersCurrentTickers["holdings"][entry]["QuoteNumber"], user=user, ticker=entry)

        print("usersQuotes**: ", usersCurrentTickers)

        return render_template("printQuotes.html", usersQuotes=usersCurrentTickers)


@app.route("/updateQuotes", methods=["GET"])
def updateQuotes():
    """Refresh users quotes or delete a specific quote"""

    if request.method == "GET":

        user = session["username"]

        usersCurrentTickers = {}

        usersCurrentTickers["holdings"] = db.execute(
            "SELECT Ticker, QuoteNumber FROM Quotes WHERE User = :user", user=user)

        # Create a dictionary of dictionaries from the list of dictionaries
        usersCurrentTickers["holdings"] = {tickerDict["Ticker"]: {
            "QuoteNumber": tickerDict["QuoteNumber"]} for tickerDict in usersCurrentTickers["holdings"]}

        if request.args.get("tickerToDelete"):
            tickerToDelete = request.args.get("tickerToDelete")

            try:
                deletedTickersQuoteNumber = usersCurrentTickers["holdings"][tickerToDelete]["QuoteNumber"]
            except KeyError:
                # Return an error if the tickerToDelete is not in usersCurrentTickers
                return apology("Ticker to Delete Not Owned by User", 403)

            db.execute("DELETE FROM Quotes WHERE User = :user AND Ticker = :tickerToDelete",
                       user=user, tickerToDelete=tickerToDelete)

            usersCurrentTickers["holdings"].pop(tickerToDelete)

            # If there are any remaining tickers in usersCurrentTickers
            if usersCurrentTickers["holdings"]:

                # Renumber remaining quotes
                for entry in usersCurrentTickers["holdings"]:
                    # If the entry's QuoteNumber is greater than the deleted quote's QuoteNumber, move it down one place
                    if usersCurrentTickers["holdings"][entry]["QuoteNumber"] > deletedTickersQuoteNumber:
                        usersCurrentTickers["holdings"][entry]["QuoteNumber"] = usersCurrentTickers["holdings"][entry]["QuoteNumber"] - 1

                        db.execute("UPDATE Quotes SET QuoteNumber = :quoteNumber WHERE User = :user AND Ticker = :ticker",
                                   quoteNumber=usersCurrentTickers["holdings"][entry]["QuoteNumber"], user=user, ticker=entry)

        # If there are any remaining tickers in usersCurrentTickers
        if usersCurrentTickers["holdings"]:
            # Update usersCurrentTickers with current pricing information
            usersCurrentTickers = prepareUsersCurrentHoldingsForDisplay(
                usersCurrentTickers)

            return render_template("printQuotes.html", usersQuotes=usersCurrentTickers)

        else:
            return render_template("printQuotes.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        errorMessage = ""

        # Log current user out (if logged in)
        session.clear()

        username = request.form.get("username")

        # Ensure username was submitted
        if not username:
            errorMessage = "Must provide a username."
        elif len(username) < 5 or len(username) > 250:
            errorMessage = "Username must be at least five characters and not more than 250 characters."
        # Check if a non-word character is present in username
        elif re.match(r"^\w*\W", username):
            errorMessage = "Username must only contain letters or numbers."

        # Check password for any errors
        if not errorMessage:

            password = request.form.get("password")

            if not password:
                errorMessage = "Must provide a password."
            elif len(password) < 5 or len(password) > 250:
                errorMessage = "Password must be at least five characters and not more than 250 characters."
            elif not re.match(r"^[\w!@#$%&]*\d", username):
                errorMessage = "Password must contain a number."
            elif not re.match(r"^\w*[!@#$%&]", username):
                errorMessage = "Password must contain a special character from list."
            elif not request.form.get("confirmation"):
                errorMessage = "Must confirm password."
            elif not password = request.form.get("confirmation"):
                errorMessage = "Password must match confirmation password."

        # Test that username matches validation constraints

        # Test for at least five characters and not more than 250 characters

        # Test for no non-word characters

        # Test that password matches validation constraints

        # Test for at least five characters and not more than 250

        # Test for at least one special character from list

        # Test for at least one numerical character

        if errorFound:
            return apology(errorMessage, 403)


        hashedPassword = generate_password_hash(password)

        test = db.execute(f"INSERT INTO users (username, hash) VALUES (:username,:hashedPassword)",
                          username=username, hashedPassword=hashedPassword)

        return redirect("/")

    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""

    thisUser = session["username"]

    usersCurrentHoldings = {}

    usersCurrentHoldings["holdings"] = db.execute(
        "SELECT Ticker, Shares FROM Holdings WHERE User = :user", user=thisUser)

    # Transform list of dictionaries into a dictionary of dictionaries
    usersCurrentHoldings["holdings"] = {dictEntry["Ticker"]: {
        "Shares": dictEntry["Shares"]} for dictEntry in usersCurrentHoldings["holdings"]}

    if request.method == "GET":

        usersCurrentHoldings = prepareUsersCurrentHoldingsForDisplay(
            usersCurrentHoldings)

        return render_template("displayHoldings.html", usersCurrentHoldings=usersCurrentHoldings, parentPage="sell")

    elif request.method == "POST":

        if not request.form.get("tickerToSell"):
            return apology("must provide symbol", 403)

        elif not request.form.get("numberOfShares"):
            return apology("must provide shares", 403)

        tickerToSell = request.form.get("tickerToSell").upper()
        sharesToSell = int(request.form.get("numberOfShares"))

        # Error if ticker symbol is too long
        if len(tickerToSell) > 5:
            return apology("Ticker length is too short.", 403)

        # Return an error if the user enters zero or fewer shares to buy
        if sharesToSell <= 0:
            return apology("Must sell at least one share.", 403)

        if tickerToSell in usersCurrentHoldings["holdings"]:

            # User does own shares of the ticker to sell

            numberOfSharesUserOwns = usersCurrentHoldings["holdings"][tickerToSell]["Shares"]

            # Create an error if the user tries to sell more shares than he/she owns
            if numberOfSharesUserOwns < sharesToSell:
                return apology("Selling more shares than you own.", 403)

            # Get the current pricing information
            thisLookupResults = lookup(tickerToSell)

            if thisLookupResults == None:
                return apology("Error getting API results", 403)

            thisSalePrice = thisLookupResults['price']

            proceeds = thisSalePrice * sharesToSell

            thisUsersCash = db.execute("SELECT cash FROM users WHERE username = :username",
                                       username=thisUser)

            # How much cash the user now has after the sale
            thisUsersCash = round(thisUsersCash[0]["cash"] + proceeds, 2)

            db.execute("UPDATE users SET cash = :newCashBalance WHERE username = :username",
                       username=thisUser, newCashBalance=thisUsersCash)

            # Delete the holding from Holdings if all the shares are being sold
            if numberOfSharesUserOwns == sharesToSell:
                db.execute("DELETE FROM Holdings WHERE User = :user AND Ticker = :tickerToDelete",
                           user=thisUser, tickerToDelete=tickerToSell)
            # Else, update the number of shares the user owns
            else:
                db.execute("UPDATE Holdings SET Shares = :shares WHERE User = :username AND Ticker = :tickerSold",
                           username=thisUser, shares=numberOfSharesUserOwns-sharesToSell, tickerSold=tickerToSell)

            # Add the sell trasaction to history
            db.execute("INSERT INTO History (Ticker, Price, DateTime, Type, User, NumberOfShares) VALUES (:ticker, :price, :dateTime, :type, :user, :numberOfShares)",
                       ticker=tickerToSell, price=thisSalePrice, dateTime=datetime.datetime.now().strftime("%d-%m-%Y %H:%M"), type="SELL", user=thisUser, numberOfShares=sharesToSell)

            returnString = f"{sharesToSell} shares of {tickerToSell} sold at {usd(thisSalePrice)} for a total of {usd(proceeds)}."

            return render_template("messageDisplay.html", message=returnString)

        return apology(f"Could not find {tickerToSell} in your portfolio.", 403)


def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)
