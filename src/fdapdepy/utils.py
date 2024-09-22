
import numpy as np

def parse_formula(formula, data):
    _formula = formula.replace(" ", "")
    seps = ["~", "+"]
    
    res = _formula.split(seps[0])
    str_obs = res[0]
    formula_rhs = res[1].split("+")
    mask_rhs = np.zeros(len(formula_rhs), dtype="b")
    for i in range(0,len(formula_rhs)):
        mask_rhs[i] = True if formula_rhs[i] in data.columns else False
    idx_smooth = mask_rhs == False
    str_smooth = str(np.array(formula_rhs)[idx_smooth][0])
    str_locs = {}
    if( len(str_smooth) > 1):
        str_locs = str_smooth.split("(")[1].split(")")[0].split(",")
    str_covs = np.delete(formula_rhs, idx_smooth)
    return {"observations": str_obs, "covariates": str_covs, "smooth": str_smooth[0], "locations": str_locs}