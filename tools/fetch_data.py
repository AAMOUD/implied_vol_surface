import yfinance as yf
import pandas as pd
from datetime import date

TICKER = "NVDA"
COLLECTION_DATE = date(2026, 6, 6)
OPTION_TYPE = "calls"

# picked these to get decent spacing across the term structure
SELECTED_EXPIRIES = [
    "2026-06-26",
    "2026-07-17",
    "2026-08-21",
    "2026-09-18",
    "2026-12-18",
    "2027-03-19",
    "2027-06-17",
]

nvda = yf.Ticker(TICKER)
current_price = nvda.fast_info["last_price"]
print(f"NVDA current price: {current_price:.2f}")

all_rows = []

for expiry in SELECTED_EXPIRIES:
    chain = nvda.option_chain(expiry)

    if OPTION_TYPE == "calls":
        df = chain.calls
    else:
        df = chain.puts

    df = df[["strike", "impliedVolatility", "volume", "openInterest"]].copy()

    # filter out illiquid options with no trading activity or basically zero IV
    df = df[df["impliedVolatility"] > 0.001]
    df = df[~((df["openInterest"] == 0) & (df["volume"] == 0))]

    # stay within 30% of spot, deep OTM options tend to have noisy IV
    lower = current_price * 0.70
    upper = current_price * 1.30
    df = df[(df["strike"] >= lower) & (df["strike"] <= upper)]
    df = df.sort_values("strike").reset_index(drop=True)

    expiry_date = date.fromisoformat(expiry)
    days = (expiry_date - COLLECTION_DATE).days
    df["maturity_days"] = days
    df["expiry"] = expiry

    print(f"{expiry} | T={days} days | {len(df)} strikes remaining after cleaning")
    all_rows.append(df)

final = pd.concat(all_rows, ignore_index=True)

output = final[["maturity_days", "strike", "impliedVolatility"]].copy()
output.columns = ["maturity_days", "strike", "iv"]

output.to_csv("data/options_data.csv", index=False)
print(output[output["maturity_days"] == 195]["strike"].values)
