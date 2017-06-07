/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   floor.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stoupin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/06 13:28:16 by stoupin           #+#    #+#             */
/*   Updated: 2017/06/06 13:28:17 by stoupin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <math.h>
#include "wolf3d.h"

void			init_floor(t_env *env, char *file_name)
{
	env->floor = load_bitmap((char*[]){file_name}, 1);
	if (!env->floor)
		exit(EXIT_FAILURE);
}

void			render_floor(t_env *env, t_coord_i c, t_coord_f angle)
{
	t_coord_f	c_floor;
	t_pix		pix;
	float		h_dist;

	h_dist = env->player.height / tan(-angle.y);
	c_floor.x = (env->player.location.x + h_dist * cos(angle.x)) / 4.;
	c_floor.y = (env->player.location.y + h_dist * sin(angle.x)) / 4.;
	c_floor.x = (c_floor.x - floor(c_floor.x)) * (env->floor->dim.x - 1);
	c_floor.y = (c_floor.y - floor(c_floor.y)) * (env->floor->dim.y - 1);

	pix = get_pix(env->floor, c_floor);
//	pix.i = 0x00ff00;

	if (h_dist > 5.)
	{
		pix.c.r /= (h_dist / 5.);
		pix.c.g /= (h_dist / 5.);
		pix.c.b /= (h_dist / 5.);
	}
	env->scene[c.y * WIDTH + c.x] = pix;
//	env->img_string[c.y * WIDTH + c.x] = pix;
}
