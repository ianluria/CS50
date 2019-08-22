import os

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd, lookupMultiple

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
    print("home!")
    return apology("home!")


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""
    return apology("TODO")


@app.route("/check", methods=["GET"])
def check():
    """Return true if username available, else false, in JSON format"""
    return jsonify("TODO")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""
    return apology("TODO")


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

        print("rows", rows)

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

    # Clear any existing list of stock quotes
    currentListOfStockPrices.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""

    if request.method == "POST":

        errorMessage = ""
        blankUserTickerEntry = False
        user = session["username"]
        thisNewQuoteNumber = 0
        usersQuoteDictEntry = {}
        multipleParametersForAPI = ""
        usersTickerSymbol = ""

        # make a copy of all the existing records for the user from the database
        usersCurrentTickers = db.execute(
            "SELECT Ticker, QuoteNumber FROM Quotes WHERE User = :user", user=user)

        usersListLength = len(usersCurrentTickers)

        # print(usersCurrentTickers)
        # return apology("aqui", 403)

        # Prepare error message to user if incomplete form
        if not request.form.get("symbol"):
            errorMessage = "Please fill out ticker symbol."
            blankUserTickerEntry = True

        # Get the ticker symbol input that the user entered if the input was not blank
        if not blankUserTickerEntry:
            usersTickerSymbol = request.form.get("symbol").upper()

        # Create error message if the length is too long?

        tickerAlreadyPresentInUsersList = False

        # If there are entries in usersCurrentTickers, test whether usersTickerSymbol is already in list
        if usersListLength > 0:

            for symbol in usersCurrentTickers:
                # Do not try to find ticker if there is already an error
                if not blankUserTickerEntry:
                    if symbol["Ticker"] == usersTickerSymbol:
                        tickerAlreadyPresentInUsersList = True

                # Build a string that will be used to query API later
                multipleParametersForAPI = multipleParametersForAPI + \
                    symbol["Ticker"] + ","

        # usersTickerSymbol is not already in list; it's safe to add it to usersCurrentTickers
        if not tickerAlreadyPresentInUsersList and not blankUserTickerEntry:

            # Assign a new QuoteNumber for usersTickerSymbol
            thisNewQuoteNumber = usersListLength + 1

            # Add usersTickerSymbol to API call string
            multipleParametersForAPI = multipleParametersForAPI + \
                usersTickerSymbol + ","

            usersQuoteDictEntry = {
                "Ticker": usersTickerSymbol, "QuoteNumber": thisNewQuoteNumber}

            # Insert usersTickerSymbol into usersCurrentTickers
            usersCurrentTickers.append(usersQuoteDictEntry)

            usersListLength = usersListLength + 1


        # Remove the trailing comma
        multipleParametersForAPI = multipleParametersForAPI[:-1]

        # Get the JSON results from calling the API with multiple parameters
        lookupResults = lookupMultiple(multipleParametersForAPI)

        # Notify user if there is an error getting prices and stop execution
        if lookupResults == None:
            return render_template("messageDisplay.html", message="Error getting results.")

        usersTickerNotPresent = True

        # Test whether the usersTickerSymbol is valid by discovering whether it is included in lookupResults
        for result in lookupResults:

            resultTickerSymbol = result["symbol"]

            # Only check for presence of usersTickerSymbol if there was not a previous error (i.e. no symbol was entered)
            if not blankUserTickerEntry:
                if resultTickerSymbol == usersTickerSymbol:
                    usersTickerNotPresent = False

            # Add price information to usersCurrentTickers
            for userQuote in usersCurrentTickers:
                if userQuote["Ticker"] == resultTickerSymbol:
                    userQuote["Price"] = result["price"]

        if not blankUserTickerEntry:
            # Return an error message if the user did enter a ticker symbol, but it was invalid (not present in API results)
            if usersTickerNotPresent:
                errorMessage = f"Unable to find ticker symbol {usersTickerSymbol}."
                # delete the usersquote from usersCurrentTickers
                usersCurrentTickers.remove(usersQuoteDictEntry)
            # Users Ticker Symbol is valid and database needs to be updated with the new ticker symbol
            else:
                listLengthIsSix = False

                # If there are more than five stocks being tracked, move each QuoteNumber down one (6 becomes 5, 1 becomes 0)
                if usersListLength > 5:

                   listLengthIsSix = True

                    for entry in usersCurrentTickers:
                        entry["QuoteNumber"] = entry["QuoteNumber"] - 1 

                    usersCurrentTickers = [entry for entry in usersCurrentTickers if entry["QuoteNumber"] > 0]    

                    # Delete all of the users existing database records
                    db.execute("DELETE FROM Quotes WHERE User = :user", user=user)

                for entry in usersCurrentTickers:
                    addToDatabase = True

                    # Only want to add ticker to database if it is not already in there
                    if not listLengthIsSix and not entry["Ticker"] == usersQuoteDictEntry["Ticker"]:
                        addToDatabase = False

                    if addToDatabase:
                        db.execute("INSERT INTO Quotes (QuoteNumber, User, Ticker) VALUES (:quoteNumber, :user, :ticker)",
                                    quoteNumber=entry["QuoteNumber"], user=user, ticker=entry["Ticker"])


        # print(usersCurrentTickers)
        # return apology("aqui", 403)
        #test123
        
        return render_template("printQuotes.html", usersQuotes=usersCurrentTickers, errorMessage=errorMessage)

        # return apology("aqui", 403)

    else:
        # return render_template("quotes.html", usersQuotes=usersQuotes)
        return render_template("quotes.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Log current user out (if logged in)
        session.clear()

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Ensure password was confirmed
        elif not request.form.get("confirmation"):
            return apology("must confirm password", 403)

        username = request.form.get("username")
        password = request.form.get("password")

        # Ensure password matches its confirmation
        if password != request.form.get("confirmation"):
            return apology("password must be confirmed", 403)

        # return redirect("/")

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
    return apology("TODO")


def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)
