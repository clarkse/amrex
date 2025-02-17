#ifndef AMREX_MLLINOP_K_H_
#define AMREX_MLLINOP_K_H_
#include <AMReX_Config.H>

#include <AMReX_FArrayBox.H>
#include <AMReX_BoundCond.H>
#include <AMReX_LO_BCTYPES.H>
#include <AMReX_LOUtil_K.H>

namespace amrex {

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_apply_bc_x (int side, Box const& box, int blen,
                         Array4<T> const& phi,
                         Array4<int const> const& mask,
                         BoundCond bct, T bcl,
                         Array4<T const> const& bcval,
                         int maxorder, T dxinv, int inhomog, int icomp) noexcept
{
    const auto lo = amrex::lbound(box);
    const auto hi = amrex::ubound(box);
    const int i = lo.x; // boundary cell
    const int s = 1-2*side;  // +1 for lo and -1 for hi
    switch (bct) {
    case AMREX_LO_NEUMANN:
    {
        for     (int k = lo.z; k <= hi.z; ++k) {
            for (int j = lo.y; j <= hi.y; ++j) {
                if (mask(i,j,k) > 0) {
                    phi(i,j,k,icomp) = phi(i+s,j,k,icomp);
                }
            }
        }
        break;
    }
    case AMREX_LO_REFLECT_ODD:
    {
        for     (int k = lo.z; k <= hi.z; ++k) {
            for (int j = lo.y; j <= hi.y; ++j) {
                if (mask(i,j,k) > 0) {
                    phi(i,j,k,icomp) = -phi(i+s,j,k,icomp);
                }
            }
        }
        break;
    }
    case AMREX_LO_DIRICHLET:
    {
        const int NX = amrex::min(blen+1, maxorder);
        GpuArray<T,4> x{{-bcl * dxinv, T(0.5), T(1.5), T(2.5)}};
        GpuArray<T,4> coef{};
        poly_interp_coeff(-T(0.5), x.data(), NX, coef.data());
        for     (int k = lo.z; k <= hi.z; ++k) {
            for (int j = lo.y; j <= hi.y; ++j) {
                if (mask(i,j,k) > 0) {
                    T tmp = T(0.0);
                    for (int m = 1; m < NX; ++m) {
                        tmp += phi(i+m*s,j,k,icomp) * coef[m];
                    }
                    phi(i,j,k,icomp) = tmp;
                    if (inhomog) {
                        phi(i,j,k,icomp) += bcval(i,j,k,icomp)*coef[0];
                    }
                }
            }
        }
        break;
    }
    default: {}
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_apply_bc_x (int side, int i, int j, int k, int blen,
                         Array4<T> const& phi,
                         Array4<int const> const& mask,
                         BoundCond bct, T bcl,
                         Array4<T const> const& bcval,
                         int maxorder, T dxinv, int inhomog, int icomp) noexcept
{
    if (mask(i,j,k) > 0) {
        const int s = 1-2*side;  // +1 for lo and -1 for hi
        switch (bct) {
        case AMREX_LO_NEUMANN:
        {
            phi(i,j,k,icomp) = phi(i+s,j,k,icomp);
            break;
        }
        case AMREX_LO_REFLECT_ODD:
        {
            phi(i,j,k,icomp) = -phi(i+s,j,k,icomp);
            break;
        }
        case AMREX_LO_DIRICHLET:
        {
            const int NX = amrex::min(blen+1, maxorder);
            GpuArray<T,4> x{{-bcl * dxinv, T(0.5), T(1.5), T(2.5)}};
            GpuArray<T,4> coef{};
            poly_interp_coeff(-T(0.5), x.data(), NX, coef.data());
            T tmp = T(0.0);
            for (int m = 1; m < NX; ++m) {
                tmp += phi(i+m*s,j,k,icomp) * coef[m];
            }
            phi(i,j,k,icomp) = tmp;
            if (inhomog) {
                phi(i,j,k,icomp) += bcval(i,j,k,icomp)*coef[0];
            }
            break;
        }
        default: {}
        }
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_apply_bc_y (int side, Box const& box, int blen,
                         Array4<T> const& phi,
                         Array4<int const> const& mask,
                         BoundCond bct, T bcl,
                         Array4<T const> const& bcval,
                         int maxorder, T dyinv, int inhomog, int icomp) noexcept
{
    const auto lo = amrex::lbound(box);
    const auto hi = amrex::ubound(box);
    const int j = lo.y; // boundary cell
    const int s = 1-2*side; // +1 for lo and -1 for hi
    switch (bct) {
    case AMREX_LO_NEUMANN:
    {
        for     (int k = lo.z; k <= hi.z; ++k) {
            for (int i = lo.x; i <= hi.x; ++i) {
                if (mask(i,j,k) > 0) {
                    phi(i,j,k,icomp) = phi(i,j+s,k,icomp);
                }
            }
        }
        break;
    }
    case AMREX_LO_REFLECT_ODD:
    {
        for     (int k = lo.z; k <= hi.z; ++k) {
            for (int i = lo.x; i <= hi.x; ++i) {
                if (mask(i,j,k) > 0) {
                    phi(i,j,k,icomp) = -phi(i,j+s,k,icomp);
                }
            }
        }
        break;
    }
    case AMREX_LO_DIRICHLET:
    {
        const int NX = amrex::min(blen+1, maxorder);
        GpuArray<T,4> x{{-bcl * dyinv, T(0.5), T(1.5), T(2.5)}};
        GpuArray<T,4> coef{};
        poly_interp_coeff(-T(0.5), x.data(), NX, coef.data());
        for     (int k = lo.z; k <= hi.z; ++k) {
            for (int i = lo.x; i <= hi.x; ++i) {
                if (mask(i,j,k) > 0) {
                    T tmp = T(0.0);
                    for (int m = 1; m < NX; ++m) {
                        tmp += phi(i,j+m*s,k,icomp) * coef[m];
                    }
                    phi(i,j,k,icomp) = tmp;
                    if (inhomog) {
                        phi(i,j,k,icomp) += bcval(i,j,k,icomp)*coef[0];
                    }
                }
            }
        }
        break;
    }
    default: {}
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_apply_bc_y (int side, int i, int j, int k, int blen,
                         Array4<T> const& phi,
                         Array4<int const> const& mask,
                         BoundCond bct, T bcl,
                         Array4<T const> const& bcval,
                         int maxorder, T dyinv, int inhomog, int icomp) noexcept
{
    if (mask(i,j,k) > 0) {
        const int s = 1-2*side; // +1 for lo and -1 for hi
        switch (bct) {
        case AMREX_LO_NEUMANN:
        {
            phi(i,j,k,icomp) = phi(i,j+s,k,icomp);
            break;
        }
        case AMREX_LO_REFLECT_ODD:
        {
            phi(i,j,k,icomp) = -phi(i,j+s,k,icomp);
            break;
        }
        case AMREX_LO_DIRICHLET:
        {
            const int NX = amrex::min(blen+1, maxorder);
            GpuArray<T,4> x{{-bcl * dyinv, T(0.5), T(1.5), T(2.5)}};
            GpuArray<T,4> coef{};
            poly_interp_coeff(-T(0.5), x.data(), NX, coef.data());
            T tmp = T(0.0);
            for (int m = 1; m < NX; ++m) {
                tmp += phi(i,j+m*s,k,icomp) * coef[m];
            }
            phi(i,j,k,icomp) = tmp;
            if (inhomog) {
                phi(i,j,k,icomp) += bcval(i,j,k,icomp)*coef[0];
            }
            break;
        }
        default: {}
        }
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_apply_bc_z (int side, Box const& box, int blen,
                         Array4<T> const& phi,
                         Array4<int const> const& mask,
                         BoundCond bct, T bcl,
                         Array4<T const> const& bcval,
                         int maxorder, T dzinv, int inhomog, int icomp) noexcept
{
    const auto lo = amrex::lbound(box);
    const auto hi = amrex::ubound(box);
    const int k = lo.z; // boundary cell
    const int s = 1-2*side; // +1 for lo and -1 for hi
    switch (bct) {
    case AMREX_LO_NEUMANN:
    {
        for     (int j = lo.y; j <= hi.y; ++j) {
            for (int i = lo.x; i <= hi.x; ++i) {
                if (mask(i,j,k) > 0) {
                    phi(i,j,k,icomp) = phi(i,j,k+s,icomp);
                }
            }
        }
        break;
    }
    case AMREX_LO_REFLECT_ODD:
    {
        for     (int j = lo.y; j <= hi.y; ++j) {
            for (int i = lo.x; i <= hi.x; ++i) {
                if (mask(i,j,k) > 0) {
                    phi(i,j,k,icomp) = -phi(i,j,k+s,icomp);
                }
            }
        }
        break;
    }
    case AMREX_LO_DIRICHLET:
    {
        const int NX = amrex::min(blen+1, maxorder);
        GpuArray<T,4> x{{-bcl * dzinv, T(0.5), T(1.5), T(2.5)}};
        GpuArray<T,4> coef{};
        poly_interp_coeff(-T(0.5), x.data(), NX, coef.data());
        for     (int j = lo.y; j <= hi.y; ++j) {
            for (int i = lo.x; i <= hi.x; ++i) {
                if (mask(i,j,k) > 0) {
                    T tmp = T(0.0);
                    for (int m = 1; m < NX; ++m) {
                        tmp += phi(i,j,k+m*s,icomp) * coef[m];
                    }
                    phi(i,j,k,icomp) = tmp;
                    if (inhomog) {
                        phi(i,j,k,icomp) += bcval(i,j,k,icomp)*coef[0];
                    }
                }
            }
        }
        break;
    }
    default: {}
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_apply_bc_z (int side, int i, int j, int k, int blen,
                         Array4<T> const& phi,
                         Array4<int const> const& mask,
                         BoundCond bct, T bcl,
                         Array4<T const> const& bcval,
                         int maxorder, T dzinv, int inhomog, int icomp) noexcept
{
    if (mask(i,j,k) > 0) {
        const int s = 1-2*side; // +1 for lo and -1 for hi
        switch (bct) {
        case AMREX_LO_NEUMANN:
        {
            phi(i,j,k,icomp) = phi(i,j,k+s,icomp);
            break;
        }
        case AMREX_LO_REFLECT_ODD:
        {
            phi(i,j,k,icomp) = -phi(i,j,k+s,icomp);
            break;
        }
        case AMREX_LO_DIRICHLET:
        {
            const int NX = amrex::min(blen+1, maxorder);
            GpuArray<T,4> x{{-bcl * dzinv, T(0.5), T(1.5), T(2.5)}};
            GpuArray<T,4> coef{};
            poly_interp_coeff(-T(0.5), x.data(), NX, coef.data());
            T tmp = T(0.0);
            for (int m = 1; m < NX; ++m) {
                tmp += phi(i,j,k+m*s,icomp) * coef[m];
            }
            phi(i,j,k,icomp) = tmp;
            if (inhomog) {
                phi(i,j,k,icomp) += bcval(i,j,k,icomp)*coef[0];
            }
            break;
        }
        default: {}
        }
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_comp_interp_coef0_x (int side, Box const& box, int blen,
                                  Array4<T> const& f,
                                  Array4<int const> const& mask,
                                  BoundCond bct, T bcl,
                                  int maxorder, T dxinv, int icomp) noexcept
{
    const auto lo = amrex::lbound(box);
    const auto hi = amrex::ubound(box);
    const int ib = lo.x; // boundary cell
    const int ii = lo.x + (1-2*side); // interior cell
    switch (bct) {
    case AMREX_LO_NEUMANN:
    {
        for     (int k = lo.z; k <= hi.z; ++k) {
            for (int j = lo.y; j <= hi.y; ++j) {
                f(ii,j,k,icomp) = T(1.0);
            }
        }
        break;
    }
    case AMREX_LO_REFLECT_ODD:
    {
        for     (int k = lo.z; k <= hi.z; ++k) {
            for (int j = lo.y; j <= hi.y; ++j) {
                f(ii,j,k,icomp) = (mask(ib,j,k) > 0) ? T(1.0) : T(0.0);
            }
        }
        break;
    }
    case AMREX_LO_DIRICHLET:
    {
        const int NX = amrex::min(blen+1, maxorder);
        GpuArray<T,4> x{{-bcl * dxinv, T(0.5), T(1.5), T(2.5)}};
        GpuArray<T,4> coef{};
        poly_interp_coeff(-T(0.5), x.data(), NX, coef.data());
        for     (int k = lo.z; k <= hi.z; ++k) {
            for (int j = lo.y; j <= hi.y; ++j) {
                f(ii,j,k,icomp) = (mask(ib,j,k) > 0) ? coef[1] : T(0.0);
            }
        }
        break;
    }
    default: {}
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_comp_interp_coef0_x (int side, int i, int j, int k, int blen,
                                  Array4<T> const& f,
                                  Array4<int const> const& mask,
                                  BoundCond bct, T bcl,
                                  int maxorder, T dxinv, int icomp) noexcept
{
    const int ii = i + (1-2*side); // interior cell
    switch (bct) {
    case AMREX_LO_NEUMANN:
    {
        f(ii,j,k,icomp) = T(1.0);
        break;
    }
    case AMREX_LO_REFLECT_ODD:
    {
        f(ii,j,k,icomp) = (mask(i,j,k) > 0) ? T(1.0) : T(0.0);
        break;
    }
    case AMREX_LO_DIRICHLET:
    {
        const int NX = amrex::min(blen+1, maxorder);
        GpuArray<T,4> x{{-bcl * dxinv, T(0.5), T(1.5), T(2.5)}};
        GpuArray<T,4> coef{};
        poly_interp_coeff(-T(0.5), x.data(), NX, coef.data());
        f(ii,j,k,icomp) = (mask(i,j,k) > 0) ? coef[1] : T(0.0);
        break;
    }
    default: {}
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_comp_interp_coef0_y (int side, Box const& box, int blen,
                                  Array4<T> const& f,
                                  Array4<int const> const& mask,
                                  BoundCond bct, T bcl,
                                  int maxorder, T dyinv, int icomp) noexcept
{
    const auto lo = amrex::lbound(box);
    const auto hi = amrex::ubound(box);
    const int jb = lo.y; // boundary cell
    const int ji = lo.y + (1-2*side); // interior cell
    switch (bct) {
    case AMREX_LO_NEUMANN:
    {
        for     (int k = lo.z; k <= hi.z; ++k) {
            for (int i = lo.x; i <= hi.x; ++i) {
                f(i,ji,k,icomp) = T(1.0);
            }
        }
        break;
    }
    case AMREX_LO_REFLECT_ODD:
    {
        for     (int k = lo.z; k <= hi.z; ++k) {
            for (int i = lo.x; i <= hi.x; ++i) {
                f(i,ji,k,icomp) = (mask(i,jb,k) > 0) ? T(1.0) : T(0.0);
            }
        }
        break;
    }
    case AMREX_LO_DIRICHLET:
    {
        const int NX = amrex::min(blen+1, maxorder);
        GpuArray<T,4> x{{-bcl * dyinv, T(0.5), T(1.5), T(2.5)}};
        GpuArray<T,4> coef{};
        poly_interp_coeff(-T(0.5), x.data(), NX, coef.data());
        for     (int k = lo.z; k <= hi.z; ++k) {
            for (int i = lo.x; i <= hi.x; ++i) {
                f(i,ji,k,icomp) = (mask(i,jb,k) > 0) ? coef[1] : T(0.0);
            }
        }
        break;
    }
    default: {}
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_comp_interp_coef0_y (int side, int i, int j, int k, int blen,
                                  Array4<T> const& f,
                                  Array4<int const> const& mask,
                                  BoundCond bct, T bcl,
                                  int maxorder, T dyinv, int icomp) noexcept
{
    const int ji = j + (1-2*side); // interior cell
    switch (bct) {
    case AMREX_LO_NEUMANN:
    {
        f(i,ji,k,icomp) = T(1.0);
        break;
    }
    case AMREX_LO_REFLECT_ODD:
    {
        f(i,ji,k,icomp) = (mask(i,j,k) > 0) ? T(1.0) : T(0.0);
        break;
    }
    case AMREX_LO_DIRICHLET:
    {
        const int NX = amrex::min(blen+1, maxorder);
        GpuArray<T,4> x{{-bcl * dyinv, T(0.5), T(1.5), T(2.5)}};
        GpuArray<T,4> coef{};
        poly_interp_coeff(-T(0.5), x.data(), NX, coef.data());
        f(i,ji,k,icomp) = (mask(i,j,k) > 0) ? coef[1] : T(0.0);
        break;
    }
    default: {}
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_comp_interp_coef0_z (int side, Box const& box, int blen,
                                  Array4<T> const& f,
                                  Array4<int const> const& mask,
                                  BoundCond bct, T bcl,
                                  int maxorder, T dzinv, int icomp) noexcept
{
    const auto lo = amrex::lbound(box);
    const auto hi = amrex::ubound(box);
    const int kb = lo.z; // bound cell
    const int ki = lo.z + (1-2*side); // interior cell
    switch (bct) {
    case AMREX_LO_NEUMANN:
    {
        for     (int j = lo.y; j <= hi.y; ++j) {
            for (int i = lo.x; i <= hi.x; ++i) {
                f(i,j,ki,icomp) = T(1.0);
            }
        }
        break;
    }
    case AMREX_LO_REFLECT_ODD:
    {
        for     (int j = lo.y; j <= hi.y; ++j) {
            for (int i = lo.x; i <= hi.x; ++i) {
                f(i,j,ki,icomp) = (mask(i,j,kb) > 0) ? T(1.0) : T(0.0);
            }
        }
        break;
    }
    case AMREX_LO_DIRICHLET:
    {
        const int NX = amrex::min(blen+1, maxorder);
        GpuArray<T,4> x{{-bcl * dzinv, T(0.5), T(1.5), T(2.5)}};
        GpuArray<T,4> coef{};
        poly_interp_coeff(-T(0.5), x.data(), NX, coef.data());
        for     (int j = lo.y; j <= hi.y; ++j) {
            for (int i = lo.x; i <= hi.x; ++i) {
                f(i,j,ki,icomp) = (mask(i,j,kb) > 0) ? coef[1] : T(0.0);
            }
        }
        break;
    }
    default: {}
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_comp_interp_coef0_z (int side, int i, int j, int k, int blen,
                                  Array4<T> const& f,
                                  Array4<int const> const& mask,
                                  BoundCond bct, T bcl,
                                  int maxorder, T dzinv, int icomp) noexcept
{
    const int ki = k + (1-2*side); // interior cell
    switch (bct) {
    case AMREX_LO_NEUMANN:
    {
        f(i,j,ki,icomp) = T(1.0);
        break;
    }
    case AMREX_LO_REFLECT_ODD:
    {
        f(i,j,ki,icomp) = (mask(i,j,k) > 0) ? T(1.0) : T(0.0);
        break;
    }
    case AMREX_LO_DIRICHLET:
    {
        const int NX = amrex::min(blen+1, maxorder);
        GpuArray<T,4> x{{-bcl * dzinv, T(0.5), T(1.5), T(2.5)}};
        GpuArray<T,4> coef{};
        poly_interp_coeff(-T(0.5), x.data(), NX, coef.data());
        f(i,j,ki,icomp) = (mask(i,j,k) > 0) ? coef[1] : T(0.0);
        break;
    }
    default: {}
    }
}

#ifdef AMREX_USE_EB

AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_comp_interp_coef0_x_eb (int side, Box const& box, int blen,
                                     Array4<Real> const& f,
                                     Array4<int const> const& mask,
                                     Array4<Real const> const& area,
                                     BoundCond bct, Real bcl,
                                     int maxorder, Real dxinv, int icomp) noexcept
{
    const auto lo = amrex::lbound(box);
    const auto hi = amrex::ubound(box);
    const int ib = lo.x; // boundary cell
    const int s = 1-2*side;  // +1 for lo and -1 for hi
    const int ii = lo.x + s; // interior cell
    switch (bct) {
    case AMREX_LO_NEUMANN:
    {
        for     (int k = lo.z; k <= hi.z; ++k) {
            for (int j = lo.y; j <= hi.y; ++j) {
                f(ii,j,k,icomp) = Real(1.0);
            }
        }
        break;
    }
    case AMREX_LO_REFLECT_ODD:
    {
        for     (int k = lo.z; k <= hi.z; ++k) {
            for (int j = lo.y; j <= hi.y; ++j) {
                f(ii,j,k,icomp) = (mask(ib,j,k) > 0) ? Real(1.0) : Real(0.0);
            }
        }
        break;
    }
    case AMREX_LO_DIRICHLET:
    {
        const int NX = amrex::min(blen+1, maxorder);
        GpuArray<Real,4> x{{-bcl * dxinv, Real(0.5), Real(1.5), Real(2.5)}};
        Array2D<Real, 0, 3, 0, 2> coef{};
        for (int r = 0; r <= maxorder-2; ++r) {
            poly_interp_coeff(-Real(0.5), x.data(), r+2, &coef(0,r));
        }
        for     (int k = lo.z; k <= hi.z; ++k) {
            for (int j = lo.y; j <= hi.y; ++j) {
                int order = 1;
                if (mask(ib,j,k) > 0) {
                    bool has_cutfaces = false;
                    for (int r = 0; r <= NX-2; ++r) {
                        Real a = area(ii+side+s*r,j,k);
                        if (a > Real(0.0)) {
                            ++order;
                            if (a < Real(1.0)) {
                                has_cutfaces = true;
                            }
                        } else {
                            break;
                        }
                    }
                    if (has_cutfaces) { order = amrex::min(2,order); }
                }
                f(ii,j,k,icomp) = (order==1) ? Real(0.0) : coef(1,order-2);
            }
        }
        break;
    }
    default: {}
    }
}

AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_comp_interp_coef0_x_eb (int side, int i, int j, int k, int blen,
                                     Array4<Real> const& f,
                                     Array4<int const> const& mask,
                                     Array4<Real const> const& area,
                                     BoundCond bct, Real bcl,
                                     int maxorder, Real dxinv, int icomp) noexcept
{
    const int s = 1-2*side;  // +1 for lo and -1 for hi
    const int ii = i + s; // interior cell
    switch (bct) {
    case AMREX_LO_NEUMANN:
    {
        f(ii,j,k,icomp) = Real(1.0);
        break;
    }
    case AMREX_LO_REFLECT_ODD:
    {
        f(ii,j,k,icomp) = (mask(i,j,k) > 0) ? Real(1.0) : Real(0.0);
        break;
    }
    case AMREX_LO_DIRICHLET:
    {
        const int NX = amrex::min(blen+1, maxorder);
        GpuArray<Real,4> x{{-bcl * dxinv, Real(0.5), Real(1.5), Real(2.5)}};
        Array2D<Real, 0, 3, 0, 2> coef{};
        for (int r = 0; r <= maxorder-2; ++r) {
            poly_interp_coeff(-Real(0.5), x.data(), r+2, &coef(0,r));
        }
        int order = 1;
        if (mask(i,j,k) > 0) {
            bool has_cutfaces = false;
            for (int r = 0; r <= NX-2; ++r) {
                Real a = area(ii+side+s*r,j,k);
                if (a > Real(0.0)) {
                    ++order;
                    if (a < Real(1.0)) {
                        has_cutfaces = true;
                    }
                } else {
                    break;
                }
            }
            if (has_cutfaces) { order = amrex::min(2,order); }
        }
        f(ii,j,k,icomp) = (order==1) ? Real(0.0) : coef(1,order-2);
        break;
    }
    default: {}
    }
}

AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_comp_interp_coef0_y_eb (int side, Box const& box, int blen,
                                     Array4<Real> const& f,
                                     Array4<int const> const& mask,
                                     Array4<Real const> const& area,
                                     BoundCond bct, Real bcl,
                                     int maxorder, Real dyinv, int icomp) noexcept
{
    const auto lo = amrex::lbound(box);
    const auto hi = amrex::ubound(box);
    const int jb = lo.y; // boundary cell
    const int s = 1-2*side;  // +1 for lo and -1 for hi
    const int ji = lo.y + s; // interior cell
    switch (bct) {
    case AMREX_LO_NEUMANN:
    {
        for     (int k = lo.z; k <= hi.z; ++k) {
            for (int i = lo.x; i <= hi.x; ++i) {
                f(i,ji,k,icomp) = Real(1.0);
            }
        }
        break;
    }
    case AMREX_LO_REFLECT_ODD:
    {
        for     (int k = lo.z; k <= hi.z; ++k) {
            for (int i = lo.x; i <= hi.x; ++i) {
                f(i,ji,k,icomp) = (mask(i,jb,k) > 0) ? Real(1.0) : Real(0.0);
            }
        }
        break;
    }
    case AMREX_LO_DIRICHLET:
    {
        const int NX = amrex::min(blen+1, maxorder);
        GpuArray<Real,4> x{{-bcl * dyinv, Real(0.5), Real(1.5), Real(2.5)}};
        Array2D<Real, 0, 3, 0, 2> coef{};
        for (int r = 0; r <= maxorder-2; ++r) {
            poly_interp_coeff(-Real(0.5), x.data(), r+2, &coef(0,r));
        }
        for     (int k = lo.z; k <= hi.z; ++k) {
            for (int i = lo.x; i <= hi.x; ++i) {
                int order = 1;
                if (mask(i,jb,k) > 0) {
                    bool has_cutfaces = false;
                    for (int r = 0; r <= NX-2; ++r) {
                        Real a = area(i,ji+side+s*r,k);
                        if (a > Real(0.0)) {
                            ++order;
                            if (a < Real(1.0)) {
                                has_cutfaces = true;
                            }
                        } else {
                            break;
                        }
                    }
                    if (has_cutfaces) { order = amrex::min(2,order); }
                }
                f(i,ji,k,icomp) = (order==1) ? Real(0.0) : coef(1,order-2);
            }
        }
        break;
    }
    default: {}
    }
}

AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_comp_interp_coef0_y_eb (int side, int i, int j, int k, int blen,
                                     Array4<Real> const& f,
                                     Array4<int const> const& mask,
                                     Array4<Real const> const& area,
                                     BoundCond bct, Real bcl,
                                     int maxorder, Real dyinv, int icomp) noexcept
{
    const int s = 1-2*side;  // +1 for lo and -1 for hi
    const int ji = j + s; // interior cell
    switch (bct) {
    case AMREX_LO_NEUMANN:
    {
        f(i,ji,k,icomp) = Real(1.0);
        break;
    }
    case AMREX_LO_REFLECT_ODD:
    {
        f(i,ji,k,icomp) = (mask(i,j,k) > 0) ? Real(1.0) : Real(0.0);
        break;
    }
    case AMREX_LO_DIRICHLET:
    {
        const int NX = amrex::min(blen+1, maxorder);
        GpuArray<Real,4> x{{-bcl * dyinv, Real(0.5), Real(1.5), Real(2.5)}};
        Array2D<Real, 0, 3, 0, 2> coef{};
        for (int r = 0; r <= maxorder-2; ++r) {
            poly_interp_coeff(-Real(0.5), x.data(), r+2, &coef(0,r));
        }
        int order = 1;
        if (mask(i,j,k) > 0) {
            bool has_cutfaces = false;
            for (int r = 0; r <= NX-2; ++r) {
                Real a = area(i,ji+side+s*r,k);
                if (a > Real(0.0)) {
                    ++order;
                    if (a < Real(1.0)) {
                        has_cutfaces = true;
                    }
                } else {
                    break;
                }
            }
            if (has_cutfaces) { order = amrex::min(2,order); }
        }
        f(i,ji,k,icomp) = (order==1) ? Real(0.0) : coef(1,order-2);
        break;
    }
    default: {}
    }
}

AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_comp_interp_coef0_z_eb (int side, Box const& box, int blen,
                                     Array4<Real> const& f,
                                     Array4<int const> const& mask,
                                     Array4<Real const> const& area,
                                     BoundCond bct, Real bcl,
                                     int maxorder, Real dzinv, int icomp) noexcept
{
    const auto lo = amrex::lbound(box);
    const auto hi = amrex::ubound(box);
    const int kb = lo.z; // bound cell
    const int s = 1-2*side;  // +1 for lo and -1 for hi
    const int ki = lo.z + s; // interior cell
    switch (bct) {
    case AMREX_LO_NEUMANN:
    {
        for     (int j = lo.y; j <= hi.y; ++j) {
            for (int i = lo.x; i <= hi.x; ++i) {
                f(i,j,ki,icomp) = Real(1.0);
            }
        }
        break;
    }
    case AMREX_LO_REFLECT_ODD:
    {
        for     (int j = lo.y; j <= hi.y; ++j) {
            for (int i = lo.x; i <= hi.x; ++i) {
                f(i,j,ki,icomp) = (mask(i,j,kb) > 0) ? Real(1.0) : Real(0.0);
            }
        }
        break;
    }
    case AMREX_LO_DIRICHLET:
    {
        const int NX = amrex::min(blen+1, maxorder);
        GpuArray<Real,4> x{{-bcl * dzinv, Real(0.5), Real(1.5), Real(2.5)}};
        Array2D<Real, 0, 3, 0, 2> coef{};
        for (int r = 0; r <= maxorder-2; ++r) {
            poly_interp_coeff(-Real(0.5), x.data(), r+2, &coef(0,r));
        }
        for     (int j = lo.y; j <= hi.y; ++j) {
            for (int i = lo.x; i <= hi.x; ++i) {
                int order = 1;
                if (mask(i,j,kb) > 0) {
                    bool has_cutfaces = false;
                    for (int r = 0; r <= NX-2; ++r) {
                        Real a = area(i,j,ki+side+s*r);
                        if (a > Real(0.0)) {
                            ++order;
                            if (a < Real(1.0)) {
                                has_cutfaces = true;
                            }
                        } else {
                            break;
                        }
                    }
                    if (has_cutfaces) { order = amrex::min(2,order); }
                }
                f(i,j,ki,icomp) = (order==1) ? Real(0.0) : coef(1,order-2);
            }
        }
        break;
    }
    default: {}
    }
}

AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_comp_interp_coef0_z_eb (int side, int i, int j, int k, int blen,
                                     Array4<Real> const& f,
                                     Array4<int const> const& mask,
                                     Array4<Real const> const& area,
                                     BoundCond bct, Real bcl,
                                     int maxorder, Real dzinv, int icomp) noexcept
{
    const int s = 1-2*side;  // +1 for lo and -1 for hi
    const int ki = k + s; // interior cell
    switch (bct) {
    case AMREX_LO_NEUMANN:
    {
        f(i,j,ki,icomp) = Real(1.0);
        break;
    }
    case AMREX_LO_REFLECT_ODD:
    {
        f(i,j,ki,icomp) = (mask(i,j,k) > 0) ? Real(1.0) : Real(0.0);
        break;
    }
    case AMREX_LO_DIRICHLET:
    {
        const int NX = amrex::min(blen+1, maxorder);
        GpuArray<Real,4> x{{-bcl * dzinv, Real(0.5), Real(1.5), Real(2.5)}};
        Array2D<Real, 0, 3, 0, 2> coef{};
        for (int r = 0; r <= maxorder-2; ++r) {
            poly_interp_coeff(-Real(0.5), x.data(), r+2, &coef(0,r));
        }
        int order = 1;
        if (mask(i,j,k) > 0) {
            bool has_cutfaces = false;
            for (int r = 0; r <= NX-2; ++r) {
                Real a = area(i,j,ki+side+s*r);
                if (a > Real(0.0)) {
                    ++order;
                    if (a < Real(1.0)) {
                        has_cutfaces = true;
                    }
                } else {
                    break;
                }
            }
            if (has_cutfaces) { order = amrex::min(2,order); }
        }
        f(i,j,ki,icomp) = (order==1) ? Real(0.0) : coef(1,order-2);
        break;
    }
    default: {}
    }
}
#endif

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_apply_innu_xlo (int i, int j, int k,
                             Array4<T> const& rhs,
                             Array4<int const> const& mask,
                             Array4<T const> const& bcoef,
                             BoundCond bct, T /*bcl*/,
                             Array4<T const> const& bcval,
                             T fac, bool has_bcoef, int icomp) noexcept
{
    if (bct == AMREX_LO_NEUMANN && mask(i,j,k) == 2) {
        T b = (has_bcoef) ? bcoef(i+1,j,k,icomp) : T(1.0);
        rhs(i+1,j,k,icomp) -= fac*b*bcval(i,j,k,icomp);
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_apply_innu_xhi (int i, int j, int k,
                             Array4<T> const& rhs,
                             Array4<int const> const& mask,
                             Array4<T const> const& bcoef,
                             BoundCond bct, T /*bcl*/,
                             Array4<T const> const& bcval,
                             T fac, bool has_bcoef, int icomp) noexcept
{
    if (bct == AMREX_LO_NEUMANN && mask(i,j,k) == 2) {
        T b = (has_bcoef) ? bcoef(i,j,k,icomp) : T(1.0);
        rhs(i-1,j,k,icomp) += fac*b*bcval(i,j,k,icomp);
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_apply_innu_ylo (int i, int j, int k,
                             Array4<T> const& rhs,
                             Array4<int const> const& mask,
                             Array4<T const> const& bcoef,
                             BoundCond bct, T /*bcl*/,
                             Array4<T const> const& bcval,
                             T fac, bool has_bcoef, int icomp) noexcept
{
    if (bct == AMREX_LO_NEUMANN && mask(i,j,k) == 2) {
        T b = (has_bcoef) ? bcoef(i,j+1,k,icomp) : T(1.0);
        rhs(i,j+1,k,icomp) -= fac*b*bcval(i,j,k,icomp);
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_apply_innu_ylo_m (int i, int j, int k,
                               Array4<T> const& rhs,
                               Array4<int const> const& mask,
                               BoundCond bct, T /*bcl*/,
                               Array4<T const> const& bcval,
                               T fac, T xlo, T dx, int icomp) noexcept
{
    if (bct == AMREX_LO_NEUMANN && mask(i,j,k) == 2) {
        T b = xlo + (i+T(0.5))*dx;
        rhs(i,j+1,k,icomp) -= fac*b*bcval(i,j,k,icomp);
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_apply_innu_yhi (int i, int j, int k,
                             Array4<T> const& rhs,
                             Array4<int const> const& mask,
                             Array4<T const> const& bcoef,
                             BoundCond bct, T /*bcl*/,
                             Array4<T const> const& bcval,
                             T fac, bool has_bcoef, int icomp) noexcept
{
    if (bct == AMREX_LO_NEUMANN && mask(i,j,k) == 2) {
        T b = (has_bcoef) ? bcoef(i,j,k,icomp) : T(1.0);
        rhs(i,j-1,k,icomp) += fac*b*bcval(i,j,k,icomp);
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_apply_innu_yhi_m (int i, int j, int k,
                               Array4<T> const& rhs,
                               Array4<int const> const& mask,
                               BoundCond bct, T /*bcl*/,
                               Array4<T const> const& bcval,
                               T fac, T xlo, T dx, int icomp) noexcept
{
    if (bct == AMREX_LO_NEUMANN && mask(i,j,k) == 2) {
        T b = xlo + (i+T(0.5))*dx;
        rhs(i,j-1,k,icomp) += fac*b*bcval(i,j,k,icomp);
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_apply_innu_zlo (int i, int j, int k,
                             Array4<T> const& rhs,
                             Array4<int const> const& mask,
                             Array4<T const> const& bcoef,
                             BoundCond bct, T /*bcl*/,
                             Array4<T const> const& bcval,
                             T fac, bool has_bcoef, int icomp) noexcept
{
    if (bct == AMREX_LO_NEUMANN && mask(i,j,k) == 2) {
        T b = (has_bcoef) ? bcoef(i,j,k+1,icomp) : T(1.0);
        rhs(i,j,k+1,icomp) -= fac*b*bcval(i,j,k,icomp);
    }
}

template <typename T>
AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
void mllinop_apply_innu_zhi (int i, int j, int k,
                             Array4<T> const& rhs,
                             Array4<int const> const& mask,
                             Array4<T const> const& bcoef,
                             BoundCond bct, T /*bcl*/,
                             Array4<T const> const& bcval,
                             T fac, bool has_bcoef, int icomp) noexcept
{
    if (bct == AMREX_LO_NEUMANN && mask(i,j,k) == 2) {
        T b = (has_bcoef) ? bcoef(i,j,k,icomp) : T(1.0);
        rhs(i,j,k-1,icomp) += fac*b*bcval(i,j,k,icomp);
    }
}

#ifdef AMREX_USE_EB

AMREX_GPU_HOST_DEVICE AMREX_FORCE_INLINE
Real get_dx_eb (Real kappa) noexcept {
    return amrex::max(Real(0.3),(kappa*kappa-Real(0.25))/(Real(2.0)*kappa));
}

#endif

}

#endif
