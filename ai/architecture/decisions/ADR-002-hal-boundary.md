# ADR-002 HAL Boundary

Status: Accepted

Application/Service는 Hardware Register를 직접 접근하지 않는다. Hardware 접근은 HAL/Driver 계층으로 제한한다.
