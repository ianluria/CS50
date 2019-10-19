import requests
import urllib.parse

from flask import redirect, render_template, request, session
from functools import wraps


def apology(message, code=400):
    """Render message as an apology to user."""
    def escape(s):
        """
        Escape special characters.

        https://github.com/jacebrowning/memegen#special-characters
        """
        for old, new in [("-", "--"), (" ", "-"), ("_", "__"), ("?", "~q"),
                         ("%", "~p"), ("#", "~h"), ("/", "~s"), ("\"", "''")]:
            s = s.replace(old, new)
        return s
    return render_template("apology.html", top=code, bottom=escape(message)), code


def login_required(f):
    """
    Decorate routes to require login.

    http://flask.pocoo.org/docs/1.0/patterns/viewdecorators/
    """
    @wraps(f)
    def decorated_function(*args, **kwargs):
        if session.get("user_id") is None:
            return redirect("/login")
        return f(*args, **kwargs)
    return decorated_function


def lookup(symbol):
    """Look up quote for symbol."""

    # Contact API
    try:
        #response = requests.get(f"https://api.iextrading.com/1.0/stock/{urllib.parse.quote_plus(symbol)}/quote")
        response = requests.get(
            f"https://cloud.iexapis.com/stable/stock/{urllib.parse.quote_plus(symbol)}/quote?token=pk_1e84952a1d2d43cfa78f95225a7217eb")
        response.raise_for_status()
    except requests.RequestException:
        return None

    # Parse response
    try:
        quote = response.json()
        return {
            "name": quote["companyName"],
            "price": float(quote["latestPrice"]),
            "symbol": quote["symbol"]
        }
    except (KeyError, TypeError, ValueError):
        return None


def usd(value):
    """Format value as USD."""
    return f"${value:,.2f}"


# https://cloud.iexapis.com/stable/tops?token=pk_1e84952a1d2d43cfa78f95225a7217eb&symbols=cinr,msft,dg

def lookupMultiple(multipleTickerString):
    """Look up multiple quotes at once."""

    # Contact API
    try:
        response = requests.get(
            f"https://cloud.iexapis.com/stable/tops/last?token=pk_1e84952a1d2d43cfa78f95225a7217eb&symbols={multipleTickerString}")

        response.raise_for_status()
    except requests.RequestException:
        return None

    # Parse response
    try:
        quote = response.json()
        return quote
    except (KeyError, TypeError, ValueError):
        return None

# Creates a string of ticker symbols to get API information for each valid symbol


def getAPIResultsWithMultipleTickers(usersCurrentHoldings):

    returnDict = {}

    parameterForAPI = ",".join(usersCurrentHoldings.keys())

    # Get the JSON results from calling the API with multiple parameters
    lookupResults = lookupMultiple(parameterForAPI)

    # Error getting results
    if lookupResults == None:
        returnDict["error"] = "Error getting API results."
        return returnDict

    # # Empty list
    # if not lookupResults:
    #     returnDict["error"]="Invalid ticker symbol."
    #     return returnDict

    returnDict["results"] = lookupResults
    return returnDict

# Takes a dictionary of dictionaries and returns it with current price information included for each ticker symbol


def prepareUsersCurrentHoldingsForDisplay(usersCurrentHoldings):

    # Return empty dict if empty dict is parameter
    if not usersCurrentHoldings:
        return usersCurrentHoldings

    # Not a dict
    if not isinstance(usersCurrentHoldings, dict):
        usersCurrentHoldings["error"] = "usersCurrentHoldings not a dictionary."
        return usersCurrentHoldings

    

    # Get current pricing information for each stock held by user
    lookupResults = getAPIResultsWithMultipleTickers(
        usersCurrentHoldings["holdings"])

    # If empty list in "results", create error
    if not lookupResults["results"]:
        lookupResults["error"] = "Unable to find ticker symbol."
        lookupResults.pop("results")

    if "error" in lookupResults:
        return lookupResults

    if "results" in lookupResults:
        # Add the current per share price to usersCurrentHoldings if ticker is in both lookupResults
        # and usersCurrentHoldings. No pricing information for a ticker means it is invalid.
        for result in lookupResults["results"]:

            usersCurrentHoldings[result["symbol"]
                                 ]["Price"] = usd(result["price"])

        returnDict["holdings"] = usersCurrentHoldings
        return returnDict
