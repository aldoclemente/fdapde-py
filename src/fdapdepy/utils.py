
def parse_formula(formula, data):
    _formula = formula.replace(" ", "")
    seps = ["~", "+"]
    
    res = _formula.split(seps[0])
    str_obs = res[0]
    print(res)
    formula_rhs = res[1].split("+")
    print(formula_rhs)
    mask_rhs = np.zeros(n_rhs, dtype="b")
    for i in range(0,len(formula_rhs)):
        mask_rhs[i] = True if formula_rhs[i] in data.columns else False
    print(mask_rhs)
    idx_smooth = which(mask_rhs, False)
    print(idx_smooth)
    str_smooth = np.array(formula_rhs[idx_smooth])
    str_covs = np.delete(formula_rhs, idx_smooth)
    return {"observations": str_obs, "covariates": str_covs, "smooth": str_smooth}